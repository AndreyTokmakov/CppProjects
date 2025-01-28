
import socket
import os

socket_path: str = "/tmp/unix_socket"

def send_and_receive():
    # Create the Unix socket client
    client = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)

    # Connect to the server
    client.connect(socket_path)

    # Send a message to the server
    message = 'Hello from the client!'
    client.sendall(message.encode())

    # Receive a response from the server
    response = client.recv(1024)
    print(f'Received response: {response.decode()}')

    # Close the connection
    client.close()


def send():
    # Create the Unix socket client
    client = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)

    # Connect to the server
    client.connect(socket_path)

    # Send a message to the server
    message = '0' * 1024
    payload: bytes = message.encode()

    for i in range(1_000_000):
        client.sendall(payload)

    # Close the connection
    client.close()


if __name__ == '__main__':
    # send_and_receive()
    send()