use p6::*;

struct MyApp {
    state: State,
}

impl MyApp {
    fn new() -> Self {
        Self(State::new(window: &Window))
    }
}

impl AppP2D for MyApp {

}

fn main() {
    State::run();
}