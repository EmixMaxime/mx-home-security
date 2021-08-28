from typing import Optional
from alarm.models import HTTPAlarmStatus
from dataclasses import dataclass
import dataclasses
from alarm.mqtt import MqttServices
from camera.messaging import CameraMessaging, HTTPCameraData, camera_messaging_factory
from utils.mqtt.mqtt_status import MqttBooleanStatus, MqttJsonStatus
from camera.messaging import CameraData


class AlarmMessaging:
    """Class to communicate with Alarm services (through mqtt).

    """
    def __init__(self, mqtt_status: MqttJsonStatus, camera_messaging: CameraMessaging):
        self._mqtt_status = mqtt_status
        self._camera_messaging = camera_messaging

    def publish_alarm_status(self, device_id: str, status: bool, http_camera_data: Optional[HTTPCameraData]) -> None:
        self._mqtt_status.publish(f'status/{MqttServices.OBJECT_DETECTION_MANAGER.value}/{device_id}', status)

        camera_data = CameraData(to_analyze=True, http=http_camera_data) if status else CameraData(to_analyze=False, http=http_camera_data)

        self._camera_messaging.publish_status(device_id, status, camera_data)


def alarm_messaging_factory(mqtt_client):
    mqtt_status = MqttJsonStatus(mqtt_client)
    camera = camera_messaging_factory(mqtt_client)

    return AlarmMessaging(mqtt_status, camera)
