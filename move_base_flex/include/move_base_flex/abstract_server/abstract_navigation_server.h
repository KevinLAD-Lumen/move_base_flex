/*
 *  Copyright 2017, Magazino GmbH, Sebastian Pütz, Jorge Santos Simón
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *
 *  3. Neither the name of the copyright holder nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  abstract_navigation_server.h
 *
 *  authors:
 *    Sebastian Pütz <spuetz@uni-osnabrueck.de>
 *    Jorge Santos Simón <santos@magazino.eu>
 *
 */

#ifndef MOVE_BASE_FLEX__ABSTRACT_NAVIGATION_SERVER_H_
#define MOVE_BASE_FLEX__ABSTRACT_NAVIGATION_SERVER_H_

#include "abstract_planner_execution.h"
#include "abstract_controller_execution.h"
#include "abstract_recovery_execution.h"

#include "move_base_flex/MoveBaseFlexConfig.h"

#include <geometry_msgs/PoseStamped.h>

#include <tf/transform_listener.h>
#include <dynamic_reconfigure/server.h>
#include <actionlib/server/simple_action_server.h>
#include <actionlib/client/simple_action_client.h>

#include <move_base_flex_msgs/GetPathAction.h>
#include <move_base_flex_msgs/ExePathAction.h>
#include <move_base_flex_msgs/RecoveryAction.h>
#include <move_base_flex_msgs/MoveBaseAction.h>

#include "move_base_flex/navigation_utility.h"
#include "move_base_flex/abstract_server/abstract_navigation_server.h"

namespace move_base_flex
{
/**
 * @defgroup abstract_server Abstract Server
 * @brief Classes belonging to the Abstract Server level.
 */

/**
 * @defgroup navigation_server Navigation Server Classes.
 * @brief Classes combining the core logic and providing concrete implementations.
 */


//! GetPath action server
typedef actionlib::SimpleActionServer<move_base_flex_msgs::GetPathAction> ActionServerGetPath;
typedef boost::shared_ptr<ActionServerGetPath> ActionServerGetPathPtr;

//! ExePath action server
typedef actionlib::SimpleActionServer<move_base_flex_msgs::ExePathAction> ActionServerExePath;
typedef boost::shared_ptr<ActionServerExePath> ActionServerExePathPtr;

//! Recovery action server
typedef actionlib::SimpleActionServer<move_base_flex_msgs::RecoveryAction> ActionServerRecovery;
typedef boost::shared_ptr<ActionServerRecovery> ActionServerRecoveryPtr;

//! MoveBase action server
typedef actionlib::SimpleActionServer<move_base_flex_msgs::MoveBaseAction> ActionServerMoveBase;
typedef boost::shared_ptr<ActionServerMoveBase> ActionServerMoveBasePtr;

//! Action clients for the MoveBase action
typedef actionlib::SimpleActionClient<move_base_flex_msgs::GetPathAction> ActionClientGetPath;
typedef actionlib::SimpleActionClient<move_base_flex_msgs::ExePathAction> ActionClientExePath;
typedef actionlib::SimpleActionClient<move_base_flex_msgs::RecoveryAction> ActionClientRecovery;

//! ExePath action topic name
const std::string name_action_exe_path = "exe_path";
//! GetPath action topic name
const std::string name_action_get_path = "get_path";
//! Recovery action topic name
const std::string name_action_recovery = "recovery";
//! MoveBase action topic name
const std::string name_action_move_base = "move_base";


typedef boost::shared_ptr<dynamic_reconfigure::Server<move_base_flex::MoveBaseFlexConfig> > DynamicReconfigureServer;

/**
 * @brief The AbstractNavigationServer is the abstract base class for all navigation servers in move_base_flex
 *        and bundles the @ref controller_execution "controller execution classes",the @ref planner_execution
 *        "planner execution classes" and the @ref recovery_execution "recovery execution classes". It provides
 *        the following action servers ActionServerGetPath -> callActionGetPath(), ActionServerExePath -> callActionExePath(),
 *        ActionServerRecovery -> callActionRecovery() and ActionServerMoveBase -> callActionMoveBase().
 *
 * @tparam LOCAL_PLANNER_BASE The base class derived from the AbstractLocalPlanner class. The local planner plugin
 *         has to implement that interface base class to be compatible with move_base_flex.
 *
 * @tparam GLOBAL_PLANNER_BASE The base class derived from the AbstractGlobalPlanner class. The global planner plugin
 *         has to implement that interface base class to be compatible with move_base_flex.
 *
 * @tparam RECOVERY_BEHAVIOR_BASE The base class derived from the AbstractRecoveryBehavior class. the recovery behavior
 *         plugin has to implement that interface base class to be compatible with move_base_flex.
 *
 * @ingroup abstract_server navigation_server
 */
template<typename LOCAL_PLANNER_BASE, typename GLOBAL_PLANNER_BASE, typename RECOVERY_BEHAVIOR_BASE>
  class AbstractNavigationServer
  {
  public:

    /**
     * @brief Constructor, reads all parameters and initializes all action servers and creates the plugin instances.
     *        Parameters are the concrete implementations of the abstract classes.
     * @param tf_listener_ptr shared pointer to the common TransformListener buffering transformations
     * @param planning_ptr shared pointer to an object of the concrete derived implementation of the AbstractPlannerExecution
     * @param moving_ptr shared pointer to an object of the concrete derived implementation of the AbstractControllerExecution
     * @param recovery_ptr shared pointer to an object of the concrete derived implementation of the AbstractRecoveryExecution
     */
    AbstractNavigationServer(const boost::shared_ptr<tf::TransformListener> &tf_listener_ptr,
                             typename AbstractPlannerExecution<GLOBAL_PLANNER_BASE>::Ptr planning_ptr,
                             typename AbstractControllerExecution<LOCAL_PLANNER_BASE>::Ptr moving_ptr,
                             typename AbstractRecoveryExecution<RECOVERY_BEHAVIOR_BASE>::Ptr recovery_ptr);

    /**
     * @brief Destructor
     */
    virtual ~AbstractNavigationServer();

    /**
     * @brief GetPath action execution method. This method will be called if the action server receives a goal
     * @param goal SimpleActionServer goal containing all necessary parameters for the action execution. See the action
     *        definitions in move_base_flex_msgs.
     */
    virtual void callActionGetPath(const move_base_flex_msgs::GetPathGoalConstPtr &goal);

    /**
     * @brief ExePath action execution method. This method will be called if the action server receives a goal
     * @param goal SimpleActionServer goal containing all necessary parameters for the action execution. See the action
     *        definitions in move_base_flex_msgs.
     */
    virtual void callActionExePath(const move_base_flex_msgs::ExePathGoalConstPtr &goal);

    /**
     * @brief Recovery action execution method. This method will be called if the action server receives a goal
     * @param goal SimpleActionServer goal containing all necessary parameters for the action execution. See the action
     *        definitions in move_base_flex_msgs.
     */
    virtual void callActionRecovery(const move_base_flex_msgs::RecoveryGoalConstPtr &goal);

    /**
     * @brief MoveBase action execution method. This method will be called if the action server receives a goal
     * @param goal SimpleActionServer goal containing all necessary parameters for the action execution. See the action
     *        definitions in move_base_flex_msgs.
     */
    virtual void callActionMoveBase(const move_base_flex_msgs::MoveBaseGoalConstPtr &goal);

    /**
     * @brief Callback function of the MoveBase action, while is executes the GetPath action part to compute a path
     * @param feedback SimpleActionServer feedback containing all feedback information for the MoveBase action. See the
     *        action definitions in move_base_flex_msgs.
     */
    virtual void actionMoveBaseExePathFeedback(const move_base_flex_msgs::ExePathFeedbackConstPtr &feedback);

    /**
     * @brief starts all action server.
     */
    virtual void startActionServers();

    /**
     * @brief initializes all server components. Initializing the plugins of the @ref planner_execution "Planner", the
     *        @ref controller_execution "Controller", and the @ref recovery_execution "Recovery Behavior".
     */
    virtual void initializeServerComponents();

    /**
     * @brief Computes the current robot pose (robot_frame_) in the global frame (global_frame_).
     * @param robot_pose Reference to the robot_pose message object to be filled.
     * @return true, if the current robot pose could be computed, false otherwise.
     */
    bool getRobotPose(geometry_msgs::PoseStamped &robot_pose);

  protected:

    /**
     * @brief Publishes the given path / plan
     * @param plan The plan, a list of stamped poses, to be published
     */
    void publishPath(std::vector<geometry_msgs::PoseStamped> &plan);

    /**
     * @brief Transforms a plan to the global frame (global_frame_) coord system.
     * @param plan Input plan to be transformed.
     * @param global_plan Output plan, which is then transformed to the global frame.
     * @return true, if the transformation succeeded, false otherwise
     */
    bool transformPlanToGlobalFrame(std::vector<geometry_msgs::PoseStamped> &plan,
                                    std::vector<geometry_msgs::PoseStamped> &global_plan);

    /**
     * @brief Reconfiguration method called by dynamic reconfigure
     * @param config Configuration parameters. See the MoveBaseFlexConfig definition.
     * @param level bit mask, which parameters are set.
     */
    virtual void reconfigure(move_base_flex::MoveBaseFlexConfig &config, uint32_t level);

    //! shared pointer to the Recovery action server
    ActionServerRecoveryPtr action_server_recovery_ptr_;

    //! shared pointer to the ExePath action server
    ActionServerExePathPtr action_server_exe_path_ptr_;

    //! shared pointer to the GetPath action server
    ActionServerGetPathPtr action_server_get_path_ptr_;

    //! shared pointer to the MoveBase action server
    ActionServerMoveBasePtr action_server_move_base_ptr_;

    //! Publisher to publish the current goal pose, which is used for path planning
    ros::Publisher current_goal_pub_;

    //! dynamic reconfigure server
    DynamicReconfigureServer dsrv_;

    //! configuration mutex for derived classes and other threads.
    boost::recursive_mutex configuration_mutex_;

    //! last configuration save
    move_base_flex::MoveBaseFlexConfig last_config_;

    //! the default parameter configuration save
    move_base_flex::MoveBaseFlexConfig default_config_;

    //! true, if the dynamic reconfigure has been setup.
    bool setup_reconfigure_;

    //! condition variable to wake up control thread
    boost::condition_variable condition_;

    //! the robot frame, to get the current robot pose in the global_frame_
    std::string robot_frame_;

    //! the global frame, in which the robot is moving
    std::string global_frame_;

    //! the tolerance for planning
    double tolerance_;

    //! timeout after tf returns without a result
    double tf_timeout_;

    //! shared pointer to the common TransformListener
    const boost::shared_ptr<tf::TransformListener> tf_listener_ptr_;

    //! shared pointer to the @ref planner_execution "PlannerExecution"
    typename AbstractPlannerExecution<GLOBAL_PLANNER_BASE>::Ptr planning_ptr_;

    //! shared pointer to the @ref controller_execution "ControllerExecution"
    typename AbstractControllerExecution<LOCAL_PLANNER_BASE>::Ptr moving_ptr_;

    //! shared pointer to the @ref recovery_execution "RecoveryExecution"
    typename AbstractRecoveryExecution<RECOVERY_BEHAVIOR_BASE>::Ptr recovery_ptr_;

    //! loop variable for the controller action
    bool active_moving_;

    //! loop variable for the planner action
    bool active_planning_;

    //! loop variable for the recovery action
    bool active_recovery_;

    //! timeout after a oscillation is detected
    ros::Duration oscillation_timeout_;

    //! minimal move distance to not detect an oscillation
    double oscillation_distance_;

    //! true, if recovery behavior for the MoveBase action is enabled.
    bool recovery_behavior_enabled_;

    //! true, if clearing rotate is allowed.
    bool clearing_rotation_allowed_;

    //! Publisher to publish the current computed path
    ros::Publisher path_pub_;

    //! Private node handle
    ros::NodeHandle private_nh_;

    //! Action client used by the move_base action
    ActionClientExePath action_client_exe_path_;

    //! Action client used by the move_base action
    ActionClientGetPath action_client_get_path_;

    //! Action client used by the move_base action
    ActionClientRecovery action_client_recovery_;

  };

} /* namespace move_base_flex */

#include "move_base_flex/abstract_server/impl/abstract_navigation_server.tcc"

#endif /* navigation_controller.h */
