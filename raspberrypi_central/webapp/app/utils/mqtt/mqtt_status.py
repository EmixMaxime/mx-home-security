import struct
import json
from decimal import Decimal

from utils.mqtt import MQTT


class MqttBooleanStatus:
    def __init__(self, mqtt_client: MQTT):
        self._mqtt_client = mqtt_client

    def publish(self, topic, message: bool) -> None:
        status_bytes = struct.pack('?', message)
        self._mqtt_client.publish(topic, status_bytes, qos=1, retain=True)


def decimal_default(obj):
    if isinstance(obj, Decimal):
        return float(obj)
    raise TypeError


class MqttJsonStatus:
    def __init__(self, mqtt_client: MQTT):
        self._mqtt_client = mqtt_client

    def publish(self, topic, msg: bool, data=None) -> None:
        payload = {
            'status': msg
        }

        if data is None:
            payload['data'] = data

        encoded_data = json.dumps(payload, default=decimal_default)

        print(encoded_data)

        self._mqtt_client.publish(topic, encoded_data, qos=1, retain=True)
