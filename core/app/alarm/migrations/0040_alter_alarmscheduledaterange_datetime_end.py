# Generated by Django 3.2 on 2021-08-14 16:26

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('alarm', '0039_alter_alarmscheduledaterange_turn_on_task'),
    ]

    operations = [
        migrations.AlterField(
            model_name='alarmscheduledaterange',
            name='datetime_end',
            field=models.DateTimeField(blank=True, null=True),
        ),
    ]
