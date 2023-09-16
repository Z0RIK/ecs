# Entity Component System

Entity system written from scratch. Code mostly based on [this article](https://www.david-colson.com/2020/02/09/making-a-simple-ecs.html) by David Colson.

However, there are two key design changes:

1. Scene scales automatically with the addition of new entities.
2. Component pools don't allocate memory for components that don't exist. David Colson himself recognizes this as a potential problem at the end of his article and discusses potential ways to solve it.

## Building

The project uses CMake, so the build process is straightforward.

### Clone the repository
``` bash
git clone https://github.com/Z0RIK/ecs
```

### Create and enter build directory
```bash
cd ecs
mkdir build
cd build
```

### Build project with Cnake
```bash
cmake -S ..
cmake --build .
```

## Example

![Life of particles screenshot](https://github.com/Z0RIK/ecs/blob/main/example/particle_of_life_screenshot.png)

The example project is a very basic implementation of a particle system.

If you want to play around with the example, I recommend building the project with `--config=Release` to improve performance.
