#pragma once

#ifndef CAMERA_H
#define CAMERA_H
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

// Abstract Camera Class
// a main purpose of this class to return a view matrix to the program
class Camera
{
public:
  glm::mat4 getViewMatrix() const;
  virtual void rotate(float yaw, float pitch) {}; // in degrees
  virtual void setPostion(const glm::vec3& position) {};
  virtual void move(const glm::vec3& offsetPos) {};

  void setFOV(const float FOV) { mFOV = FOV; };

  // getters
  const glm::vec3& getLook() const { return mLook; };
  const glm::vec3& getRight() const { return mRight; };
  const glm::vec3& getUp() const { return mUp; };
  const glm::vec3& getPosition() const { return mPostion; };
  float getFOV() const { return mFOV; }; // in degrees  

protected:

  Camera();

  // data for lookat() that creates a 4x4 matrix for the camera
  glm::vec3 mPostion{};
  glm::vec3 mTarPos{};
  glm::vec3 mUp{};
  glm::vec3 mLook{};
  glm::vec3 mRight{};
  const glm::vec3 WORLD_UP{};

  // Camera settings. We use Eulers angles in radians because it's being used by glm when multiplying matrices. Angles should be radians
  float mYaw{};
  float mPitch{};

  // camera parameters
  float mFOV{};
};

class FPSCamera : public Camera
{
public:

  // ctr with default camera settings
  FPSCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), float yaw = glm::pi<float>(), float pitch = 0.0f);

  virtual void rotate(float yaw, float pitch); // in degrees
  virtual void setPostion(const glm::vec3& position);
  virtual void move(const glm::vec3& OffsetPos);

private:

  void updateCameraVectors();
};

class OrbitCamera : public Camera
{
public:

  OrbitCamera();

  virtual void rotate(float yaw, float pitch); // in degrees
  void setLookAt(const glm::vec3& target); // target where to look at
  void setRadius(float radius); // Radius determines zoom


private:

  // private method to manipulate of camera vectors
  void updateCameraVectors();
  float mRadius{};


};



#endif // !CAMERA_H
