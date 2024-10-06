package bgu.spl.net.srv;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;


public class ConnectionController<T> implements Connections<T> {

    private Map<Integer, ConnectionHandler<T>> connections;
    private Map<String, Integer> userToConnection; // userName to connectionId
    private Map<Integer, String> connectionToUser; // connectionId to UserName
    private List<String> channels;
    private UserController userController;
    private int countConnectionId;

    public ConnectionController() {
        this.connections = new HashMap<Integer, ConnectionHandler<T>>();
        this.userToConnection = new HashMap<String, Integer>();
        this.connectionToUser = new HashMap<Integer, String>();
        this.userController = new UserController();
        this.channels = new LinkedList<String>();
        this.countConnectionId=0;
    }

    public boolean send(int connectionId, T msg) {
        try {
            connections.get(connectionId).send(msg);
            return true;
        } catch (Exception ex) {
            return false;
        }
    }
    
    public void send(String channel, T msg) {
        for (String userName : userToConnection.keySet()) {
            if (userController.isSubscriptionExist(userName, channel)) {
                send(userToConnection.get(userName), msg);
            }
        }
    }

    public void disconnect(int connectionId) {
        String userName = connectionToUser.get(connectionId);
        userController.unsubscribeFromAllChannels(userName);
        userToConnection.remove(userName);
        connectionToUser.remove(connectionId);
        connections.remove(connectionId);
    }

    public boolean isSubscriptionExist(int connectionId, int subscriptionId){
        String userName =  connectionToUser.get(connectionId);
        return userController.isSubscriptionExist(userName, subscriptionId);
    }

    public boolean unsubscribeFromChannel(int connectionId, int subscriptionId){
        String userName =  connectionToUser.get(connectionId);
        return userController.isSubscriptionExist(userName, subscriptionId);
    }

    public boolean isChannelExist(String channelName){
        return channels.contains(channelName);
    }

    public void subscribe(int connectionId, int subscriptionId, String channelName){
        String userName =  connectionToUser.get(connectionId);
        userController.subscribe(userName, subscriptionId, channelName);
    }

    public void addChannelAndSubscribe(int connectionId, int subscriptionId, String channelName){
        channels.add(channelName);
        subscribe(connectionId, subscriptionId, channelName);
    }

    public int getSubscriptionId(int connectionId,String channelName){
        String userName =  connectionToUser.get(connectionId);
        return userController.getUser(userName).getSubscriptionId(channelName);
    }

    public boolean isUserExist(String userName){
        return userController.isUserExist(userName);
    }

    public boolean createUser(int connectionId,String userName, String password){
        userController.createUser(userName, password);
        logging(connectionId, userName, password);
        return true;
    }

    public boolean isUserIsLogged(String userName){
        return userToConnection.containsKey(userName);
    }

    public boolean checkPassword(String userName, String password){
        return userController.checkPassword(userName, password);
    }

    public void logging(int connectionId,String username,String password){
        connectionToUser.put(connectionId, username);
        userToConnection.put(username, connectionId);
    }

    public void addConnection(int connectionId,ConnectionHandler<T> handler){
        connections.put(connectionId, handler);
    }
    public void removeConnection(int connectionId){
        connections.remove(connectionId);
    }

    public int getCountConnectionId(){
        int temp=countConnectionId;
        countConnectionId=countConnectionId+1;
        return temp;
    }
}
