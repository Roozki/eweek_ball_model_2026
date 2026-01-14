# Importing necessary modules from the ROS2 package
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    # LaunchDescription is a class that stores the launch configuration
    launch_description = LaunchDescription()

    # Creating a node using the Node class from launch_ros.actions
    # The Node class is used to start a ROS node from a launch file
    # Parameters:
    # - package: the package where the node executable is located
    # - executable: the name of the node executable
    # - name: (optional) a custom name for the node
    # - output: where to output the node's log messages, e.g., 'screen'
    speaker_node = Node(
        package='rover_sounds',
        executable='speaker_node',
        name='speaker_node',
        output='screen'
    )
    eweek_node = Node(
        package='eweek',
        executable='app',
        name='eweek_app',
        output='screen'
    )


    # Adding the node to the launch description
    # This step is required for every node you want to launch
    launch_description.add_action(speaker_node)
    launch_description.add_action(eweek_node)

    # The function returns the LaunchDescription object, which will be executed by the ROS2 launch system
    return launch_description
