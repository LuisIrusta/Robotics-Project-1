#Launcher file for odometer, tf_error and rviz
import os

from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():
    route = get_package_share_directory('first_project')
    rviz_config = os.path.join(route, 'rviz', 'config.rviz')
    return LaunchDescription([
        Node(
            package = 'first_project',
            executable = 'odometer',
            name = 'odometer',
            output='screen',
            parameters=[{'use_sim_time': True}],
        ),
        Node(
            package = 'first_project',
            executable = 'tf_error',
            name = 'tf_error',
            output='screen',
            parameters=[{'use_sim_time': True}],
        ),
        Node(
            package = 'rviz2',
            executable = 'rviz2',
            name = 'rviz2',
            arguments = ['-d', rviz_config],
            output = 'screen',
            parameters=[{'use_sim_time': True}],
        ),
    ])
