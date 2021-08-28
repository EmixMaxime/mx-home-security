# Generated by Django 3.2 on 2021-08-28 10:03

from django.db import migrations, models
import django.db.models.deletion


class Migration(migrations.Migration):

    dependencies = [
        ('alarm', '0038_auto_20210813_0928_squashed_0040_alter_alarmscheduledaterange_datetime_end'),
    ]

    operations = [
        migrations.CreateModel(
            name='HTTPAlarmStatus',
            fields=[
                ('alarmstatus_ptr', models.OneToOneField(auto_created=True, on_delete=django.db.models.deletion.CASCADE, parent_link=True, primary_key=True, serialize=False, to='alarm.alarmstatus')),
                ('user', models.CharField(max_length=100)),
                ('password', models.CharField(max_length=100)),
                ('endpoint', models.URLField()),
            ],
            bases=('alarm.alarmstatus',),
        ),
    ]
