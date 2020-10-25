from celery import shared_task
from alarm import models as alarm_models
from devices.models import Device
from notification.tasks import send_message
from utils.mqtt import mqtt_factory
from .external.motion import save_motion
from .messaging import speaker_messaging_factory
from alarm.models import AlarmSchedule


@shared_task(name="security.play_sound")
def play_sound(device_id: str):
    # device = device_models.Device.objects.get(device_id=device_id)
    mqtt_client = mqtt_factory()

    speaker = speaker_messaging_factory(mqtt_client)
    speaker.publish_speaker_status(device_id, True)


@shared_task(name="security.camera_motion_picture")
def camera_motion_picture(device_id: str, picture_path: str, event_ref: str, status: str):
    device = Device.objects.get(device_id=device_id)

    picture = alarm_models.CameraMotionDetectedPicture(device=device, picture_path=picture_path, event_ref=event_ref, is_motion=status)
    picture.save()

    kwargs = {
        'picture_path': picture_path
    }

    send_message.apply_async(kwargs=kwargs)


@shared_task(name="security.camera_motion_detected")
def camera_motion_detected(device_id: str, seen_in: dict, event_ref: str, status: bool):
    device, motion = save_motion(device_id, seen_in, event_ref, status)

    if device is None:
        # the motion is already save in db, and so the notification should have been already send.
        return None

    location = device.location

    if motion.is_motion:
        message = f'Une présence étrangère a été détectée chez vous depuis {device_id} {location.structure} {location.sub_structure}'
    else:
        message = f"La présence étrangère précédemment détectée chez vous depuis {device_id} {location.structure} {location.sub_structure} ne l'est plus."

    kwargs = {
        'message': message
    }

    """
    TODO: check if this is a correct way to create & run multiple jobs.
    ! They are not related, they have to run in total parallel.
    
    send_message can run multiple time for one notification See issue #94
    If something goes wrong in this function after the real send notification, then
    it will retry it -> notify the user multiple times.
    """
    send_message.apply_async(kwargs=kwargs)
    # play_sound.apply_async(kwargs={'device_id': device_id})

    mqtt_client = mqtt_factory()
    speaker = speaker_messaging_factory(mqtt_client)
    speaker.publish_speaker_status(device_id, motion.is_motion)


@shared_task(name="alarm.set_alarm_off")
def set_alarm_off(alarm_status_uui):
    schedule = AlarmSchedule.objects.get(uuid=alarm_status_uui)
    for alarm_status in schedule.alarm_statuses:
        alarm_status = False
        alarm_status.save()


@shared_task(name="alarm.set_alarm_on")
def set_alarm_on(alarm_status_uui):
    schedule = AlarmSchedule.objects.get(uuid=alarm_status_uui)
    for alarm_status in schedule.alarm_statuses:
        alarm_status = True
        alarm_status.save()

