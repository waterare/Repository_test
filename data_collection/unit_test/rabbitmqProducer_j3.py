import pika
import json
import random
import time

class RabbitMqProducer():
    def __init__(self):
        self.user="admin"
        self.pwd="123456"
        self.queue = 'chuneng'
        self.credentials = pika.PlainCredentials(self.user, self.pwd)
        self.connection = pika.BlockingConnection(pika.ConnectionParameters(host = '175.154.142.113',port =5670,virtual_host = '/',credentials = self.credentials))
        self.channel = self.connection.channel()

    def producer(self,message):
        self.channel.queue_declare(queue = self.queue, durable = True)
        self.channel.basic_publish(exchange="",routing_key=self.queue,body=message.encode('utf-8'))

RMD=RabbitMqProducer()

while True:
    capacity_1 = 12.00 + random.uniform(-0.5,0.5) 
    voltage_1 = 380.00 + random.uniform(-2,2)  
    current_1 = 10.00 + random.uniform(-1,1) 
    capacity_1 = format(capacity_1, '.2f')
    voltage_1 = format(voltage_1, '.2f')
    current_1 = format(current_1, '.2f')

    capacity_2 = 12.00 + random.uniform(-0.5,0.5) 
    voltage_2 = 380.00 + random.uniform(-2,2)  
    current_2 = 10.00 + random.uniform(-1,1)  
    capacity_2 = format(capacity_2, '.2f')
    voltage_2 = format(voltage_2, '.2f')
    current_2 = format(current_2, '.2f')

    capacity_3 = 12.00 + random.uniform(-0.5,0.5) 
    voltage_3 = 380.00 + random.uniform(-2,2)  
    current_3 = 10.00 + random.uniform(-1,1)  
    capacity_3 = format(capacity_3, '.2f')
    voltage_3 = format(voltage_3, '.2f')
    current_3 = format(current_3, '.2f')

    battery_info = {"Battery_module": "8s4p","Rated_capacity": "12.00","Rated_voltage": 380.00,"Battery": [{ "capacity":capacity_1 , "voltage":voltage_1 , 'current': current_1 }, { "capacity":capacity_2 , "voltage":voltage_2 , 'current': current_2 }, { "capacity":capacity_3 , "voltage":voltage_3 , 'current': current_3 }, ]}
    json_data = json.dumps(battery_info)
    message=json_data
    RMD.producer(message)
    time.sleep(1)

RMD.channel.close()