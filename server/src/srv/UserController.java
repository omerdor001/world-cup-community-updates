package bgu.spl.net.srv;

import java.util.HashMap;
import java.util.Map;

public class UserController {
    private Map<String,User> users;

    public UserController(){
       this.users = new HashMap<String,User>();
    }

    public boolean createUser(String userName, String password){
        if(!users.containsKey(userName)){
            users.put(userName, new User(userName, password));
            return true;
        }
        else{
            return false;
        }
    }

    public boolean isUserExist(String userName){
        return users.containsKey(userName);
    }

    public User getUser(String userName){
        return users.get(userName);
    }

    public boolean isSubscriptionExist(String userName, int subscriptionId){
        return users.get(userName).isSubscriptionExist(subscriptionId);
    }

    public boolean isSubscriptionExist(String userName, String channelName){
        return users.get(userName).isSubscriptionExist(channelName);
    }

    public void unsubscribeFromAllChannels(String userName){
        users.get(userName).unsubscribeFromAllChannels();
    }

    public void unsubscribeFromChannel(String userName, int subscriptionId){
        users.get(userName).unsubscribeFromChannel(subscriptionId);
    }

    public void subscribe(String userName, int subscriptionId, String channelName){
        users.get(userName).addSubscription(subscriptionId, channelName);
    }

    public boolean checkPassword(String userName, String password){
        return users.get(userName).getPassword().equals(password);
    }

    
}
