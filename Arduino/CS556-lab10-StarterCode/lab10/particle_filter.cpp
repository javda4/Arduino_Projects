#include <Pololu3piPlus32U4.h>
#include <Gaussian.h>
#include "particle_filter.h"
#include "sonar.h"
using namespace Pololu3piPlus32U4;

Sonar sonar(4);

/* Initialize particle filter and particle list */
ParticleFilter::ParticleFilter(int lenOfMap, int num_particles, float translation_variance, float rotation_variance, float measurement_variance) {
  //TODO: set class variables to passed variables

  _lenOfMap = lenOfMap;
  _num_particles = num_particles;
  _translation_variance = translation_variance;
  _rotation_variance = rotation_variance;
  _measurement_variance = measurement_variance;

  _iter = 0; //Iterator

  _x_est;
  _y_est;
  _angle_est;

  Map _mp = Map();

  //generate particles with random location and rotation using _lenOfMap for x, y, and angle.
  //All probabilities will be the same at the start.
  //TODO: fill in "..."
  for(uint8_t i=0;i<_num_particles; i++){
    _particle_list[i].x = (float)random(_lenOfMap);
    _particle_list[i].y = (float)random(_lenOfMap);
    _particle_list[i].angle = (float)random(_lenOfMap) / 10.0; // 0 to ~6.28
    _particle_list[i].probability =  1.0 / _num_particles;
  }
}

/* Propagate motion of particles */
void ParticleFilter::move_particles(float dx, float dy, float dtheta){
//Apply motion to each particle by using the Gaussian class.
//You will need to utilize _translation_variance and _rotation_variance
//TODO: Put code under here

//Then add the current _particle_list[i] angle value with dtheta + random rotational
//And add the current _particle_list[i] x value with dx + random translation times the appropriate sin/cos angle value
//Do the same for y
//Consider using a for loop
//TODO: Put code under here
for (uint8_t i = 0; i < _num_particles; i++) {
    _particle_list[i].angle += dtheta + Gaussian(0, _rotation_variance).random();
    _particle_list[i].x += dx + Gaussian(0, _translation_variance).random() * cos(_particle_list[i].angle);
    _particle_list[i].y += dy + Gaussian(0, _translation_variance).random() * sin(_particle_list[i].angle);

    // _particle_list[i].x = x;
    // _particle_list[i].y = y;
    // _particle_list[i].angle = angle;
  }
}


/* Calculate particle posterior probabilities*/
void ParticleFilter::measure(){
  float norm_factor=0;
  float maxprob=-99;
  //Put next chunk in for loop:
    // compute what the distance should be, if particle position is accurate
    /*If the robot uses closest_distance to calculate posterior probabilities in a particle filter,
      the return of this function helps assess the likelihood of the particle's position given the map layout.
      
      For example, if a particle's estimated sensor reading (distance to nearest obstacle) 
      aligns with the robot's actual sensor reading, 
      this particle is likely to be close to the robot’s true position.*/
      //TODO: Fill in "..."
  for (uint8_t i = 0; i < _num_particles; i++) {
    float origin[2] = {(float)(_particle_list[i].x/(_lenOfMap/3)),(float)(_particle_list[i].y/(_lenOfMap/3))};
    float particleDist = _mp.closest_distance(origin, _particle_list[i].angle);
    float measured = sonar.readDist();

      // compute the probability P[measured z | robot @ x]
      //TODO: Put code under here
    Gaussian Ns = Gaussian(particleDist*(_lenOfMap/3),_measurement_variance);


      /* compute the probability P[robot@x | measured]
     NOTE: This is not a probability, since we don't know P[measured z]
     Hence we normalize afterwards */
     //TODO: Put code under here

    //float prob = Gaussian(particleDist, _measurement_variance).plot(measured);

    _particle_list[i].probability = Ns.plot(measured)*_particle_list[i].probability;



     // normalize probabilities (take P[measured z into account])
     //TODO: Put code under here
     norm_factor += _particle_list[i].probability;
if (maxprob < _particle_list[i].probability){
      maxprob = _particle_list[i].probability;
    }

  
  }


  //End of for loop

  //take each probability and normalize by norm_factor (in a for loop)
  //TODO: Put code under here
 for (uint8_t i = 0; i < _num_particles; i++) {
    _particle_list[i].probability /= norm_factor;
  }



  //Outside all for loops, call resample(maxprob) at the end of function
  //TODO: Put code under here
 resample(maxprob);
}



/* Resample particles */
void ParticleFilter::resample(float maxprob){
  float b = 0.0;
  float norm_tot=0;
  uint8_t maxind=0;
  uint8_t index = (int)random(_num_particles);

  Particle temp_particles[_num_particles];

  float beta = 0.0;
  float max_weight = maxprob;

  for (uint8_t i = 0; i < _num_particles; i++) {
    beta += ((float)random(1000) / 1000.0) * 2.0 * max_weight;

    while (beta > _particle_list[index].probability) {
      beta -= _particle_list[index].probability;
      index = (index + 1) % _num_particles;
    }

    temp_particles[i] = _particle_list[index];
    temp_particles[i].probability = 1.0 / _num_particles;
  }

  for (uint8_t i = 0; i < _num_particles; i++) {
    _particle_list[i] = temp_particles[i];
  }
}


/* Print particle position and probabilities to serial monitor */
void ParticleFilter::print_particles(){
  Serial.print("Iteration: ");
  Serial.println(_iter);
  estimate_position();

  for (uint8_t i = 0; i < _num_particles; i++) {
    Serial.print("P[");
    Serial.print(i);
    Serial.print("] x: ");
    Serial.print(_particle_list[i].x);
    Serial.print(" y: ");
    Serial.print(_particle_list[i].y);
    Serial.print(" angle: ");
    Serial.print(_particle_list[i].angle);
    Serial.print(" prob: ");
    Serial.println(_particle_list[i].probability);
  }

  Serial.print("Estimated position: x=");
  Serial.print(_x_est);
  Serial.print(", y=");
  Serial.print(_y_est);
  Serial.print(", angle=");
  Serial.println(_angle_est);
}


/* Estimate the position of the robot using posterior probabilities */
void ParticleFilter::estimate_position(){
  for (uint8_t i = 0; i < _num_particles; i++) {
    _x_est = _particle_list[i].x * _particle_list[i].probability;
    _y_est = _particle_list[i].y * _particle_list[i].probability;
    _angle_est = _particle_list[i].angle * _particle_list[i].probability;
  }
}
