# Generated by Django 3.2 on 2022-02-27 13:15

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('camera', '0013_alter_cameramotionvideo_last_record'),
    ]

    operations = [
        migrations.AlterField(
            model_name='cameramotionvideo',
            name='number_records',
            field=models.PositiveIntegerField(default=1),
        ),
    ]
