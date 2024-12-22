# Car website

## Websocket

### Motor data

All motor data requests start with `00` (the op code, because i might want to add more operations later)

If the client send to server the server will update the motors and notify all connected clients. 

If the server send to client the server tells the client that the motors direction has been updated. 

The data is composed of four bits, the first to for the left motor, the second for the right. The first bit for each motor is for the direction where `0` is reverse and `1` is forward. The second bit is for power where `0` means the motor is off and `1` that its on. 

To take up 8 bits the data is padded with zeros at the end. 