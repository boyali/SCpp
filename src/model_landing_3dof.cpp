#include "model_landing_3dof.h"



model_landing_3dof::StateVector model_landing_3dof::ode(const StateVector &x, const ControlVector &u) {

    const double throttle = u(0, 0);
    const double gimbalAngle = u(1, 0);
    const double vx = x(2, 0);
    const double vy = x(3, 0);
    const double theta = x(4, 0);
    const double dtheta = x(5, 0);
    const double x0 = TWR*g*throttle;
    const double x1 = gimbalAngle + theta;

    StateVector f;
    f(0, 0) = vx;
    f(1, 0) = vy;
    f(2, 0) = x0*sin(x1);
    f(3, 0) = g*(TWR*throttle*cos(x1) - 1);
    f(4, 0) = dtheta;
    f(5, 0) = -rTB*x0*sin(gimbalAngle)/(rG*rG);


    return f;
}

model_landing_3dof::StateMatrix model_landing_3dof::state_jacobian(const StateVector &x, const ControlVector &u) {

    const double throttle = u(0, 0);
    const double gimbalAngle = u(1, 0);
    const double theta = x(4, 0);
    const double x0 = TWR*g*throttle;
    const double x1 = gimbalAngle + theta;

    StateMatrix A;
    A.setZero();
    A(0, 2) = 1;
    A(1, 3) = 1;
    A(2, 4) = x0*cos(x1);
    A(3, 4) = -x0*sin(x1);
    A(4, 5) = 1;


    return A;
}



model_landing_3dof::ControlMatrix model_landing_3dof::control_jacobian(const StateVector &x, const ControlVector &u) {

    const double throttle = u(0, 0);
    const double gimbalAngle = u(1, 0);
    const double theta = x(4, 0);
    const double x0 = TWR*g;
    const double x1 = gimbalAngle + theta;
    const double x2 = x0*sin(x1);
    const double x3 = x0*cos(x1);
    const double x4 = TWR*g*rTB/(rG*rG);

    ControlMatrix B;
    B.setZero();
    B(2, 0) = x2;
    B(2, 1) = throttle*x3;
    B(3, 0) = x3;
    B(3, 1) = -throttle*x2;
    B(5, 0) = -x4*sin(gimbalAngle);
    B(5, 1) = -throttle*x4*cos(gimbalAngle);


    return B; 
}



void model_landing_3dof::add_application_constraints(EcosWrapper &solver, size_t K) {

    auto var = [&](const string &name, const vector<size_t> &indices){ return solver.get_variable(name,indices); };
    auto param = [](double &param_value){ return optimization_problem::Parameter(&param_value); };
    
    // initial state
    solver.add_constraint( (-1.0) * var("X", {0, 0}) + param(rx_init) == 0.0 );
    solver.add_constraint( (-1.0) * var("X", {1, 0}) + param(ry_init) == 0.0 );
    solver.add_constraint( (-1.0) * var("X", {2, 0}) + param(vx_init) == 0.0 );
    solver.add_constraint( (-1.0) * var("X", {3, 0}) + param(vy_init) == 0.0 );
    solver.add_constraint( (-1.0) * var("X", {4, 0}) + param(theta_init) == 0.0 );
    solver.add_constraint( (-1.0) * var("X", {5, 0}) + param(dtheta_init) == 0.0 );


    // final state
    solver.add_constraint( (1.0) * var("X", {0, K-1}) == 0.0 );
    solver.add_constraint( (1.0) * var("X", {1, K-1}) == 0.0 );
    solver.add_constraint( (1.0) * var("X", {2, K-1}) == 0.0 );
    solver.add_constraint( (1.0) * var("X", {3, K-1}) == 0.0 );
    solver.add_constraint( (1.0) * var("X", {4, K-1}) == 0.0 );
    solver.add_constraint( (1.0) * var("X", {5, K-1}) == 0.0 );

    // glide slope cone
    // TODO


    // control constraints
    for (size_t k = 0; k < K; ++k) {

        // throttle limits
        solver.add_constraint( ( 1.0) * var("U", {0, k}) + (-0.1) >= (0.0) );
        solver.add_constraint( (-1.0) * var("U", {0, k}) + (1.0) >= (0.0) );

        // gimbal limits
        solver.add_constraint( ( 1.0) * var("U", {1, k}) + (max_gimbal_angle) >= (0.0) );
        solver.add_constraint( (-1.0) * var("U", {1, k}) + (max_gimbal_angle) >= (0.0) );
    }
}

model_landing_3dof::StateVector model_landing_3dof::get_random_state() {
    StateVector X;
    X.setRandom();
    X *= 10;
    return X;
}

model_landing_3dof::ControlVector model_landing_3dof::get_random_input() {
    ControlVector U;
    U.setRandom();
    return U;
}