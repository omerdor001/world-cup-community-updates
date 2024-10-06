package bgu.spl.net.srv;

import java.util.HashMap;
import java.util.Map;

public class User {

    private String userName;
    private String password;
    private Map<Integer,String> subscriptions;


    public User(String userName,String password){
        this.userName =  userName;
        this.password = password;
        this.subscriptions = new HashMap<Integer,String>(); //subscriptionId , channelName
    }

    public String getUserName(){
        return userName;
    }

    public String getPassword(){
        return password;
    }

    public Map<Integer,String> getSubscriptions(){
        return subscriptions;
    } 

    public void addSubscription(int subscriptionId, String channelName){
          subscriptions.put(subscriptionId, channelName);
    } 

    public boolean isSubscriptionExist(String channelName){
        for (String channel : subscriptions.values()) {
            if(channelName.equals(channel)){
                return true;
            }
        }
        return false;
    } 

    public boolean isSubscriptionExist(int subscriptionId){
        return subscriptions.containsKey(subscriptionId);
    } 

    public void unsubscribeFromAllChannels(){
        subscriptions.clear();
    }

    public void unsubscribeFromChannel(int subscriptionId){
        subscriptions.remove(subscriptionId);
    }

    public int getSubscriptionId(String channelName){
        for(int subId:subscriptions.keySet()){
            if(subscriptions.get(subId).equals(channelName)){
                return subId;
            }
        }
        return -1;
    }
    
}
