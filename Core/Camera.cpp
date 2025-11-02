#include "Camera.h"
#include "glm/gtx/transform.hpp"

Camera::Camera()
  :mPostion(glm::vec3(0.0f, 0.0f, 0.0f)),
  mTarPos(glm::vec3(0.0f, 0.0f, 0.0f)),
  mUp(glm::vec3(0.0f, 1.0f, 0.0f)),
  mRight(glm::vec3(0.0f, 0.0f, 0.0f)),
  WORLD_UP(0.0f, 1.0f, 0.0f),
  mYaw(glm::pi<float>()),
  mPitch(0.0f),
  mFOV(45.0f)
{

}


glm::mat4 Camera::getViewMatrix() const
{
  // just return a view matrix for the camera
  return glm::lookAt(mPostion, mTarPos, mUp);
}


// Orbit Camera
OrbitCamera::OrbitCamera()
  : mRadius(10.0f)
{}

void OrbitCamera::rotate(float yaw, float pitch)
{
  // convert to radians first because glm supports rads only
  mYaw = glm::radians(yaw);
  mPitch = glm::radians(pitch);

  // because of we use radians for pitch we use pi and 0.1f is some kind of threshold to reach very close to the max clamp value
  // and constrain the pitch
  mPitch = glm::clamp(mPitch, -glm::pi<float>() / 2.0f + 0.1f, glm::pi<float>() / 2.0f - 0.1f);

  // once we set new pitch we need to update vectors of the camera
  // it needs to be called every time we we rotate the camera
  updateCameraVectors();
}

void OrbitCamera::setLookAt(const glm::vec3 & target)
{
  mTarPos = target;
}

void OrbitCamera::setRadius(float radius)
{
  mRadius = glm::clamp(radius, 2.0f, 80.0f);
}

void OrbitCamera::updateCameraVectors()
{
  // this method is MAGIC when camera update happens

  // Spherical to Cartesian coordinates (we need to convert to coord system that supported by OpenGL)
  // NOTE: our coordinates sys Y up not Z
  mPostion.x = mTarPos.x + mRadius * cosf(mPitch) * sinf(mYaw);
  mPostion.y = mTarPos.y + mRadius * sinf(mPitch);
  mPostion.z = mTarPos.z + mRadius * cosf(mPitch) * cosf(mYaw);


}

FPSCamera::FPSCamera(glm::vec3 position, float yaw, float pitch)
{
  mPostion = position;
  mYaw = yaw;
  mPitch = pitch;
}

void FPSCamera::rotate(float yaw, float pitch)
{
  // convert to radians first because glm supports rads only
  // and incrementally sets the postion
  mYaw += glm::radians(yaw);
  mPitch += glm::radians(pitch);

  // because of we use radians for pitch we use pi and 0.1f is some kind of threshold to reach very close to the max clamp value
  // and constrain the pitch
  mPitch = glm::clamp(mPitch, -glm::pi<float>() / 2.0f + 0.1f, glm::pi<float>() / 2.0f - 0.1f);

  // once we set new pitch we need to update vectors of the camera
  // it needs to be called every time we we rotate the camera
  updateCameraVectors();
}

void FPSCamera::setPostion(const glm::vec3 & position)
{
  mPostion = position;
}

void FPSCamera::move(const glm::vec3 & offsetPos)
{
  // just simple moving a camera with offset incrementally
  mPostion += offsetPos;
  // after each camera move, as always, we need to update camera vectors
  // Up/Right/Forward should always be correct, so, we update camera vectors evert time once a user moved the camera
  updateCameraVectors();
}

void FPSCamera::updateCameraVectors()
{
  // this method is MAGIC when camera update happens

  // Spherical to Cartesian coordinates (we need to convert to coord system that supported by OpenGL)
  // NOTE: our coordinates sys Y up not Z

  // calculate the view direction vector based on yaw and pitch angles (roll not considered)
  glm::vec3 look{};
  look.x = cosf(mPitch) * sinf(mYaw);
  look.y = sinf(mPitch);
  look.z = cosf(mPitch) * cosf(mYaw);

  // camera look (where camera watches is normalized a vector got here (look)
  mLook = glm::normalize(look);

  // Re-calculate Right and Up vector. For simplicity the Right vector will be assumed horizontal
  // w.r.t. the world's Up vector  
  mRight = glm::normalize(glm::cross(mLook, WORLD_UP)); // calculate Right Vector with Cross Product
  mUp = glm::normalize(glm::cross(mRight, mLook)); // calculate Up Vector with Cross Product

  // update Target Position based on calculations
  mTarPos = mPostion + mLook; // positon of the camera added to a camera look vector
}
