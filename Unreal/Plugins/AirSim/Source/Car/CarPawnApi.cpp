#include "CarPawnApi.h"
#include "AirBlueprintLib.h"

CarPawnApi::CarPawnApi(UWheeledVehicleMovementComponent* movement, const msr::airlib::GeoPoint& home_geopoint)
    : movement_(movement), home_geopoint_(home_geopoint)
{
}

bool CarPawnApi::armDisarm(bool arm)
{
    //TODO: implement arming for car
    unused(arm);
    return true;
}

void CarPawnApi::setCarControls(const CarApiBase::CarControls& controls)
{
    last_controls_ = controls;

    if (!controls.is_manual_gear && movement_->GetTargetGear() < 0)
        movement_->SetTargetGear(0, true); //in auto gear we must have gear >= 0
    if (controls.is_manual_gear && movement_->GetTargetGear() != controls.manual_gear)
        movement_->SetTargetGear(controls.manual_gear, controls.gear_immediate);

    movement_->SetThrottleInput(controls.throttle);
    movement_->SetSteeringInput(controls.steering);
    movement_->SetBrakeInput(controls.brake);
    movement_->SetHandbrakeInput(controls.handbrake);
    movement_->SetUseAutoGears(!controls.is_manual_gear);
}

const msr::airlib::CarApiBase::CarControls& CarPawnApi::getCarControls() const
{
    return last_controls_;
}

msr::airlib::CarApiBase::CarState CarPawnApi::getCarState() const
{
    CarApiBase::CarState state(
        movement_->GetForwardSpeed() / 100, //cm/s -> m/s
        movement_->GetCurrentGear(),
        movement_->GetEngineRotationSpeed(),
        movement_->GetEngineMaxRotationSpeed(),
        last_controls_.handbrake,
        msr::airlib::ClockFactory::get()->nowNanos()
    );
    return state;
}

void CarPawnApi::reset()
{
    last_controls_ = CarControls();
    //auto phys_comps = UAirBlueprintLib::getPhysicsComponents(pawn_->getPawn());
    UAirBlueprintLib::RunCommandOnGameThread([this]() {
        //pawn_->reset();

        //for (auto* phys_comp : phys_comps) {
        //    phys_comp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
        //    phys_comp->SetPhysicsLinearVelocity(FVector::ZeroVector);
        //    phys_comp->SetSimulatePhysics(false);
        //}
        //movement_->ResetMoveState();
        //movement_->SetActive(false);
        //movement_->SetActive(true, true);
        setCarControls(CarControls());
        
    }, true);

    //UAirBlueprintLib::RunCommandOnGameThread([this, &phys_comps]() {
    //    for (auto* phys_comp : phys_comps)
    //        phys_comp->SetSimulatePhysics(true);
    //}, true);
}

msr::airlib::GeoPoint CarPawnApi::getHomeGeoPoint() const
{
    return home_geopoint_;
}

void CarPawnApi::enableApiControl(bool is_enabled)
{
    if (api_control_enabled_ != is_enabled) {
        last_controls_ = CarControls();
        api_control_enabled_ = is_enabled;
    }
}

bool CarPawnApi::isApiControlEnabled() const
{
    return api_control_enabled_;
}

CarPawnApi::~CarPawnApi() = default;
