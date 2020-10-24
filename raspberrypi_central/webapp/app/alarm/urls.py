from django.urls import include, path

from alarm.views.alarm_status_views import AlarmStatusList, AlarmStatusUpdate, AlarmStatusCreate
from alarm.views.camera_motion_views import CameraMotionDetectedList
from alarm.views.camera_roi_views import CameraRectangleROICreate, CameraRectangleROIUpdate, CameraRectangleROIList

app_name = 'alarm'

status_patterns = [
    path('', AlarmStatusList.as_view(), name='status-list'),
    path('<int:pk>/edit', AlarmStatusUpdate.as_view(), name='status-edit'),
    path('add', AlarmStatusCreate.as_view(), name='status-add')
]

roi_patterns = [
    path('', CameraRectangleROIList.as_view(), name='camera_rectangle_roi-list'),
    path('<int:pk>/edit', CameraRectangleROIUpdate.as_view(), name='camera_rectangle_roi-edit'),
    path('add', CameraRectangleROICreate.as_view(), name='camera_rectangle_roi-add'),
]

motions_pattern = [
    path('', CameraMotionDetectedList.as_view(), name='camera_motion_detected-list'),
]

urlpatterns = [
    path('status/', include(status_patterns)),
    path('camera_roi/', include(roi_patterns)),
    path('camera_motion/', include(motions_pattern)),
]
