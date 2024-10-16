# ChatApp A Project by Onkar Parag Kulkarni

Student ID: 1286580

## How to build project

Step 1) Just extract the project folder in any directory where you have enough space.

Step 2) Open the ChatApp folder and you will see another ChatApp named folder in it. Open that as well.

Step 3) Then open the .sln file.

Step 4) Go to solution explorer and right click on solution and click on build the solution.

Step 5) After building the solution you can be able to see a x64 folder under ChatApp/ChatApp.

Step 6) Open the x64 folder inside that you will be able to see a Debug folder in which the .exe files of server and clients are placed, named as ChatAppServer.exe and ChatAppClient.exe

## Run the Project

Step 1) Run the ChatAppServer.exe file first and then run the ChatAppClient.exe.

Step 2) After opening ChatAppServer.exe it should show something like this

```
getaddrinfo successful!

Socket created successfully!

Socket binding successful!

Listening to socket successful!
```

Step 3) Open the ChatAppClient.exe it should show something like this

```
getaddrinfo successful!

Enter your name:
```

Step 4) Enter your name in the provided field.

Step 5) Now it should show something like this

```
getaddrinfo successful!

Enter your name: [UserName1]

Connected to room as [UserName1]...

Type '/quit' to leave the chat.
```

Step 6) You can repeat steps 3, 4 and 5 to open another client.

Step 7) Now first client should show this

```
getaddrinfo successful!

Enter your name: [UserName1]

Connected to room as [UserName]...

Type '/quit' to leave the chat.

[UserName2] has joined the room!
```

Step 8) And something like this should show on you ChatAppServer.exe

```
getaddrinfo successful!

Socket created successfully!

Socket binding successful!

Listening to socket successful!

------------ [UserName1] has joined the room. ------------

------------ [UserName2] has joined the room. ------------
```

Step 9) Type a message in [UserName1] console and hit enter after writing message!

Step 10) [UserName2] should see something like this

```
getaddrinfo successful!

Enter your name: [UserName2]

Connected to room as [UserName2]...

Type '/quit' to leave the chat.

[UserName1]: [Message from UserName1]
```

Step 11) Also you should see something like this in ChatAppServer.exe

```
getaddrinfo successful!

Socket created successfully!

Socket binding successful!

Listening to socket successful!

------------ [UserName1] has joined the room. ------------

------------ [UserName2] has joined the room. ------------

PacketSize:24

MessageType:1

MessageLength:12

Message:[UserName1]: [Message from UserName1]

Step 12) If [UserName1] and [UserName2] sends several messages it would look like this in ChatAppClient.exe of [UserName2]

```
getaddrinfo successful!

Enter your name: [UserName2]

Connected to room as [UserName2]...

Type '/quit' to leave the chat.

[UserName1]: [1st Message from UserName1]

hiii      // THIS IS MESSAGE FROM [UserName2]

[UserName1]: [2nd Message from UserName1]
```

Step 13) If [UserName1] and [UserName2] sends several messages it would look like this in ChatAppServer.exe

```
getaddrinfo successful!

Socket created successfully!

Socket binding successful!

Listening to socket successful!

------------ [UserName1] has joined the room. ------------

------------ [UserName2] has joined the room. ------------

PacketSize:24

MessageType:1

MessageLength:12

Message:[UserName1]: [1st Message from UserName1]

PacketSize:27

MessageType:1

MessageLength:15

Message:[UserName2]: [1st Message from UserName2]

PacketSize:31

MessageType:1

MessageLength:19

Message:[UserName1]: [2nd Message from UserName1]
```

Step 14) Enter "/quit" in chat from any client to quit the ChatAppClient.exe and it should quit the ChatAppClient.exe and will show something like this on another ChatAppClient.exe

```
getaddrinfo successful!

Enter your name: [UserName1]

Connected to room as [UserName1]...

Type '/quit' to leave the chat.

[UserName2] has joined the room!

[Message from UserName1]

[UserName2]: [Message from UserName2]

[Message from UserName1]

[UserName2] has disconnected!
```

And it should show something like this on ChatAppServer.exe

```
getaddrinfo successful!

Socket created successfully!

Socket binding successful!

Listening to socket successful!

------------ [UserName1] has joined the room. ------------

------------ [UserName2] has joined the room. ------------

PacketSize:24

MessageType:1

MessageLength:12

Message:[UserName1]: [1st Message from UserName1]

PacketSize:27

MessageType:1

MessageLength:15

Message:[UserName2]: [1st Message from UserName2]

PacketSize:31

MessageType:1

MessageLength:19

Message:[UserName1]: [2nd Message from UserName1]

PacketSize:39

MessageType:1

MessageLength:27

Message:------------ [UserName2] disconnected! ------------
```

Step 15) The connected user can still send the messages and those will be shown in the ChatAppServer.exe

## Thank You For Using My ChatApp!
