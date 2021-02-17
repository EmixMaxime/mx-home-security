from django.contrib import admin

from camera.models import CameraMotionDetected, CameraMotionDetectedPicture, CameraRectangleROI, CameraROI, \
    CameraMotionDetectedBoundingBox, CameraMotionVideo

admin.site.register(CameraMotionDetected)
admin.site.register(CameraMotionDetectedPicture)
admin.site.register(CameraRectangleROI)
admin.site.register(CameraROI)
admin.site.register(CameraMotionDetectedBoundingBox)
admin.site.register(CameraMotionVideo)
