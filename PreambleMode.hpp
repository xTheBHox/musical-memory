#include "Mode.hpp"
#include "Sound.hpp"

#include <memory>

struct PreambleMode : Mode {

	PreambleMode();
	virtual ~PreambleMode();

  virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
  virtual void update(float elapsed) override;
  virtual void draw(glm::uvec2 const &drawable_size) override;

  std::shared_ptr< Sound::PlayingSample > playing;

};
