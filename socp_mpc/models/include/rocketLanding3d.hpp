#pragma once

#include <string>
#include <random>

#include "systemModel.hpp"
#include "ecosWrapper.hpp"
#include "parameterServer.hpp"

#include "rocketLanding3dDefinitions.hpp"

namespace rocketlanding3d
{

/**
 * @brief A 3D rocket landing model.
 * 
 */
class RocketLanding3D : public SystemModel<STATE_DIM_, INPUT_DIM_, PARAM_DIM_>
{
public:
    RocketLanding3D();

    static std::string getModelName()
    {
        return "RocketLanding3D";
    }

    void getTimeHorizon(double &T) override;

    void systemFlowMap(
        const state_vector_ad_t &x,
        const input_vector_ad_t &u,
        const param_vector_ad_t &par,
        state_vector_ad_t &f) override;

    void getInitializedTrajectory(Eigen::MatrixXd &X,
                                  Eigen::MatrixXd &U) override;

    void addApplicationConstraints(op::SecondOrderConeProgram &socp,
                                   Eigen::MatrixXd &X0,
                                   Eigen::MatrixXd &U0) override;

    void nondimensionalize() override;

    void redimensionalize() override;

    void nondimensionalizeTrajectory(Eigen::MatrixXd &X,
                                     Eigen::MatrixXd &U) override;

    void redimensionalizeTrajectory(Eigen::MatrixXd &X,
                                    Eigen::MatrixXd &U) override;

    void getNewModelParameters(param_vector_t &param) override;

    struct Parameters
    {
        Eigen::Vector3d g_I;
        Eigen::Vector3d J_B;
        Eigen::Vector3d r_T_B;
        double alpha_m;
        double T_min;
        double T_max;

        double gimbal_max;
        double theta_max;
        double gamma_gs;
        double w_B_max;

        Eigen::Vector3d rpy_init;

        state_vector_t x_init;
        state_vector_t x_final;
        double final_time_guess;

        double m_scale, r_scale;

        void randomizeInitialState();

        void loadFromFile();

        void nondimensionalize();

        void redimensionalize();

        void nondimensionalizeTrajectory(Eigen::MatrixXd &X, Eigen::MatrixXd &U) const;

        void redimensionalizeTrajectory(Eigen::MatrixXd &X, Eigen::MatrixXd &U) const;
    } p;
};

} // namespace rocketlanding3d