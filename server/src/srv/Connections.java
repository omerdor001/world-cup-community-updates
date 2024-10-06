package bgu.spl.net.srv;


public interface Connections<T> {

    boolean send(int connectionId, T msg);

    void send(String channel, T msg);

    void disconnect(int connectionId);

    boolean isSubscriptionExist(int connectionId, int subscriptionId);

    boolean unsubscribeFromChannel(int connectionId, int subscriptionId);

    boolean isChannelExist(String channelName);

    void subscribe(int connectionId, int subscriptionId, String channelName);

    void addChannelAndSubscribe(int connectionId, int subscriptionId, String channelName);

    int getSubscriptionId(int connectionId,String channelName);

    boolean isUserExist(String userName);

    boolean createUser(int connectionId,String userName, String password);

    boolean isUserIsLogged(String userName);

    boolean checkPassword(String userName, String password);

    void addConnection(int connectionId,ConnectionHandler<T> handler);

    void logging(int connectionId,String username,String password);

    int getCountConnectionId();

    void removeConnection(int connectionId);

}
