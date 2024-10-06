package bgu.spl.net.impl.stomp;

import java.util.HashMap;
import java.util.Map;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.Connections;

public class StompMessagingProtocolImpl implements StompMessagingProtocol<String> {

    private boolean shouldTerminate = false;
    private Connections<String> connections;
    private int connectionId;
    private int messageId;
    private Map<String, String> headers;

    public StompMessagingProtocolImpl() {
        messageId = 0;
    }

    @Override
    public void start(int connectionId, Connections<String> connections) {
        this.connectionId = connectionId;
        this.connections = connections;
        this.headers = new HashMap<String, String>();
    }

    @Override
    public void process(String message) {
        String[] lines = message.split("\n");
        String toSend;
        if (lines[0].equals("CONNECT")) {
            boolean empty = false;
            for (int i = 1; i < lines.length && !empty; i++) {
                if (lines[i] == "") {
                    empty = true;
                } else {
                    String[] line = lines[i].split(":");
                    line[0] = line[0] + ":";
                    headers.put(line[0], line[1]);
                }
            }
            if (!headers.containsKey("accept - version :")) {
                toSend = "ERROR" + "\n" + "message: malformed frame received" + "\n"
                        + "The message:" + "\n" + "-----" + "\n" + (String) message + "\n" +
                        "-----" + "\n" + "The version is not fit ." + "\n" +
                        "needs to be 1.2 ." + "\n";
                connections.send(connectionId, toSend);
                connections.removeConnection(connectionId);
                shouldTerminate = true;
            } else if (!headers.containsKey("host :")) {
                toSend = "ERROR" + "\n" + "message: malformed frame received" + "\n"
                        + "The message:" + "\n" + "-----" + "\n" + (String) message + "\n" +
                        "-----" + "\n" + "The host is not fit," + "\n" +
                        "needs to be stomp . cs . bgu . ac . il ." + "\n";
                connections.send(connectionId, toSend);
                connections.removeConnection(connectionId);
                shouldTerminate = true;
            } else if (!headers.containsKey("login :")) {
                toSend = "ERROR" + "\n" + "message: malformed frame received" + "\n"
                        + "The message:" + "\n" + "-----" + "\n" + (String) message + "\n" +
                        "-----" + "\n" + "Login line is missing ." + "\n";
                connections.send(connectionId, toSend);
                connections.removeConnection(connectionId);
                shouldTerminate = true;
            } else if (!headers.containsKey("passcode :")) {
                toSend = "ERROR" + "\n" + "message: malformed frame received" + "\n"
                        + "The message:" + "\n" + "-----" + "\n" + (String) message + "\n" +
                        "-----" + "\n" + "Passcode line is missing ." + "\n";
                connections.send(connectionId, toSend);
                connections.removeConnection(connectionId);
                shouldTerminate = true;
            } else {
                int index_u = lines[3].indexOf(':');
                String username = lines[3].substring(index_u + 1);
                int index_p = lines[4].indexOf(':');
                String password = lines[4].substring(index_p + 1);
                if (!connections.isUserExist(username)) {
                    connections.createUser(connectionId, username, password);
                    toSend = "CONNECTED" + "\n" + "version :1.2" +"\n" +"\n";
                    connections.send(connectionId, toSend);
                } else if (connections.isUserIsLogged(username)) {
                    toSend = "ERROR" + "\n" + "message: User already logged in" + "\n"
                            + "The message:" + "\n" + "-----" + "\n" + (String) message + "\n" +
                            "-----" + "\n";
                    connections.send(connectionId, toSend);
                    connections.removeConnection(connectionId);
                    shouldTerminate = true;
                } else if (!connections.checkPassword(username, password)) {
                    toSend = "ERROR" + "\n" + "message: Password is incorrect" + "\n"
                            + "The message:" + "\n" + "-----" + "\n" + (String) message + "\n" +
                            "-----" + "\n";
                    connections.send(connectionId, toSend);
                    connections.removeConnection(connectionId);
                    shouldTerminate = true;
                } else {
                    connections.logging(connectionId, username, password);
                    toSend = "CONNECTED" + "\n" + "version :1.2" + "\n" + "\n";
                    connections.send(connectionId, toSend);
                }
            }
            headers.clear();
        } else if (lines[0].equals("SEND")) {
            boolean empty = false;
            for (int i = 1; i < lines.length && !empty; i++) {
                if (lines[i] == "") {
                    empty = true;
                } else {
                    String[] line = lines[i].split(":");
                    line[0] = line[0] + ":";
                    if(line.length==1){
                        headers.put(line[0], " ");
                    }
                    else headers.put(line[0], line[1]);
                }
            }
            if (!headers.containsKey("destination :")) {
                toSend = "ERROR" + "\n" + "message: malformed frame received" + "\n"
                        + "The message:" + "\n" + "-----" + "\n" + (String) message + "\n" +
                        "-----" + "\n" + "Did not contain a subscription id header ," + "\n" +
                        "which is REQUIRED for a SEND frame ." + "\n";
                connections.send(connectionId, toSend);
                connections.disconnect(connectionId);
                shouldTerminate = true;
            } else {
                int index = lines[1].indexOf('/');
                String destination = lines[1].substring(index + 1);
                if (!connections.isChannelExist(destination)) {
                    toSend = "ERROR" + "\n" + "message: malformed frame received" + "\n"
                            + "The message:" + "\n" + "-----" + "\n" + (String) message + "\n" +
                            "-----" + "\n" + "Channel does no exist." + "\n" +
                            "\n";
                    connections.send(connectionId, toSend);
                    connections.disconnect(connectionId);
                    shouldTerminate = true;
                } else {
                    int subscriptionId = connections.getSubscriptionId(connectionId, destination);
                    toSend = "MESSAGE" + "\n" +"subscription :" + subscriptionId + "\n" + "message - id :" + messageId
                            +"\n" + "destination :/" + destination + "\n" + "\n"+ (String) message + "\n";
                    connections.send(destination, toSend);
                    messageId = messageId + 1;
                }
            }
            headers.clear();
        } else if (lines[0].equals("SUBSCRIBE")) {
            boolean empty = false;
            for (int i = 1; i < lines.length && !empty; i++) {
                if (lines[i] == "") {
                    empty = true;
                } else {
                    String[] line = lines[i].split(":");
                    line[0] = line[0] + ":";
                    headers.put(line[0], line[1]);
                }
            }
            if (!headers.containsKey("destination :")) {
                toSend = "ERROR" + "\n" + "message: malformed frame received" + "\n"
                        + "The message:" + "\n" + "-----" + "\n" + (String) message + "\n" +
                        "-----" + "\n" + "Did not contain a subscription id header ," + "\n" +
                        "which is REQUIRED for a SUBSCRIBE frame ." + "\n";
                connections.send(connectionId, toSend);
                connections.disconnect(connectionId);
                shouldTerminate = true;
            }
            if (!headers.containsKey("id :")) {
                toSend = "ERROR" + "\n" + "message: malformed frame received" + "\n"
                        + "The message:" + "\n" + "-----" + "\n" + (String) message + "\n" +
                        "-----" + "\n" + "Did not contain a subscription id header ," + "\n" +
                        "which is REQUIRED for a SUBSCRIBE frame ." + "\n";
                connections.send(connectionId, toSend);
                connections.disconnect(connectionId);
                shouldTerminate = true;
            }
            if (!headers.containsKey("receipt :")) {
                toSend = "ERROR" + "\n" + "message: malformed frame received" + "\n"
                        + "The message:" + "\n" + "-----" + "\n" + (String) message + "\n" +
                        "-----" + "\n" + "Did not contain a receipt header ," + "\n" +
                        "which is REQUIRED for a SUBSCRIBE frame ." + "\n";
                connections.send(connectionId, toSend);
                connections.disconnect(connectionId);
                shouldTerminate = true;
            } else {
                int index = lines[1].indexOf('/');
                String destination = lines[1].substring(index + 1);
                index = lines[2].indexOf(':');
                String subId = lines[2].substring(index + 1);
                int subscriptionId = Integer.parseInt(subId);
                if (connections.isChannelExist(destination)) {
                    connections.subscribe(connectionId, subscriptionId, destination);
                } else {
                    connections.addChannelAndSubscribe(connectionId, subscriptionId, destination);
                }
                index = lines[3].indexOf(':');
                String receiptId = lines[3].substring(index + 1);
                toSend = "RECEIPT" + "\n" + "receipt - id :" + receiptId + "\n" + "\n";
                connections.send(connectionId, toSend);
                headers.clear();
            }
            headers.clear();
        } else if (lines[0].equals("UNSUBSCRIBE")) {
            boolean empty = false;
            for (int i = 1; i < lines.length && !empty; i++) {
                if (lines[i] == "") {
                    empty = true;
                } else {
                    String[] line = lines[i].split(":");
                    line[0] = line[0] + ":";
                    headers.put(line[0], line[1]);

                }
            }
            if (!headers.containsKey("id :")) {
                toSend = "ERROR" + "\n" + "message: malformed frame received" + "\n"
                        + "The message:" + "\n" + "-----" + "\n" + (String) message + "\n" +
                        "-----" + "\n" + "Did not contain a subscription id header ," + "\n" +
                        "which is REQUIRED for a UNSUBSCRIBE frame ." + "\n";
                connections.send(connectionId, toSend);
                connections.disconnect(connectionId);
                shouldTerminate = true;
            }
            if (!headers.containsKey("receipt :")) {
                toSend = "ERROR" + "\n" + "message: malformed frame received" + "\n"
                        + "The message:" + "\n" + "-----" + "\n" + (String) message + "\n" +
                        "-----" + "\n" + "Did not contain a receipt header ," + "\n" +
                        "which is REQUIRED for a UNSUBSCRIBE frame ." + "\n";
                connections.send(connectionId, toSend);
                connections.disconnect(connectionId);
                shouldTerminate = true;
            } else {
                int index = lines[1].indexOf(':');
                String subscriptionId = lines[1].substring(index + 1);
                int subId = Integer.parseInt(subscriptionId);
                if (connections.isSubscriptionExist(connectionId, subId)) {
                    connections.unsubscribeFromChannel(connectionId, subId);
                    index = lines[2].indexOf(':');
                    String receiptId = lines[2].substring(index + 1);
                    toSend = "RECEIPT" + "\n" + "receipt - id :" + receiptId+ "\n"  + "\n";
                    connections.send(connectionId, toSend);
                } else {
                    toSend = "ERROR" + "\n" + "message: malformed frame received" + "\n"
                            + "The message:" + "\n" + "-----" + "\n" + (String) message + "\n" +
                            "-----" + "\n" + "User is not subscribe to this channel." + "\n" +"\n";
                    connections.send(connectionId, toSend);
                    connections.disconnect(connectionId);
                    shouldTerminate = true;
                }
            }
            headers.clear();
        } else if (lines[0].equals("DISCONNECT")) {
            boolean empty = false;
            for (int i = 1; i < lines.length && !empty; i++) {
                if (lines[i] == "") {
                    empty = true;
                } else {
                    String[] line = lines[i].split(":");
                    line[0] = line[0] + ":";
                    headers.put(line[0], line[1]);

                }
            }
            if (!headers.containsKey("receipt :")) {
                toSend = "ERROR" + "\n" + "message: malformed frame received" + "\n"
                        + "The message:" + "\n" + "-----" + "\n" + (String) message + "\n" +
                        "-----" + "\n" + "Did not contain a receipt header ," + "\n" +
                        "which is REQUIRED for a DISCONNECT frame ." + "\n";
                connections.send(connectionId, toSend);
                connections.disconnect(connectionId);
            } else {
                int index = lines[1].indexOf(':');
                String receiptId = lines[1].substring(index + 1);
                toSend = "RECEIPT" + "\n" + "receipt - id :" + receiptId + "\n" + "\n";
                connections.send(connectionId, toSend);
                connections.disconnect(connectionId);
            }
            shouldTerminate = true;
            headers.clear();
        } else {
            toSend = "ERROR" + "\n" + "message: malformed frame received" + "\n"
                    + "The message:" + "\n" + "-----" + "\n" + (String) message + "\n" +
                    "-----" + "\n" + "Did not contain a destination header ," + "\n" +
                    "which is a MUST for a STOMP frame." + "\n";
            connections.send(connectionId, toSend);
            connections.disconnect(connectionId);
            shouldTerminate = true;
        }
    }

    /**
     * @return true if the connection should be terminated
     */
    public boolean shouldTerminate() {
        return shouldTerminate;
    }

    public int getConnectionId(){
        return connectionId;
    }

}
