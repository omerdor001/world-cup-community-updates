package bgu.spl.net.impl.stomp;

import bgu.spl.net.impl.echo.LineMessageEncoderDecoder;
import bgu.spl.net.srv.ConnectionController;
import bgu.spl.net.srv.Connections;
import bgu.spl.net.srv.Server;

public class StompServer {

    public static void main(String[] args) {
        Connections<String> connections=new ConnectionController<String>();
        int port=Integer.parseInt(args[0]);


        if(args[1].equals("tpc")){
            Server.threadPerClient(
                    port,
                    () -> new StompMessagingProtocolImpl(),
                    LineMessageEncoderDecoder::new
            ).serve(connections);
        }
        else if(args[1].equals("reactor")){
            Server.reactor(
                    Runtime.getRuntime().availableProcessors(),
                    port,
                    () -> new StompMessagingProtocolImpl(),
                    LineMessageEncoderDecoder::new
            ).serve(connections);

        }

        }
        }

                
           

