
import socket
import os

port, host = 52525, '0.0.0.0'

def load_test(messages_to_send: int):
    message = '0' * 1024
    payload: bytes = message.encode()
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        for i in range(messages_to_send):
            bytes_send: int = sock.sendto(payload, (host, port))
            # print(bytes_send)


if __name__ == '__main__':
    load_test(2_000_000)