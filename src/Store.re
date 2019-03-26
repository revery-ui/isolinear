open Isolinear_Types;

module Make = (ModelImpl: Model) => {
  type state = ModelImpl.state;
  type actions = ModelImpl.actions;

  let updater = ModelImpl.updater;

  let currentState: ref(state) = ref(ModelImpl.initialState);

  let getState = () => currentState^;

  let _subscribers = ref([]);

  let poll = () => ();

  let subscribe = fn => {
    _subscribers := [fn, ..._subscribers^];

    () => {
      _subscribers := List.filter(f => f != fn, _subscribers^);
    };
  };

  let dispatch = action => {
    let (newState, effect) = updater(currentState^, action);
    currentState := newState;

    List.iter(f => f(newState, effect), _subscribers^);
  };
};
