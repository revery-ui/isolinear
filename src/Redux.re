open Redux_Types;


module Make = (ModelImpl: Model) => {
    type state = ModelImpl.state;
    type action = ModelImpl.action;

    let reducer = ModelImpl.reducer;

    let currentState: ref(state) = ref(ModelImpl.initialState);

    let getState = () => currentState^;

    let _subscribers = ref([]);

    let poll = () => ();

    let subscribe = (fn) => {

        _subscribers := [fn, ..._subscribers^];

        () => {
            _subscribers := List.filter(f => f != fn, _subscribers^);
        }
    };

    let dispatch = (action) => {
        let newState = reducer(currentState^, action);
        currentState := newState;

        List.iter((f) => f(newState), _subscribers^);
    };
};

module Enhance = (StoreImpl: Store, EnhancerImpl: StoreEnhancer with type action = StoreImpl.action and type state = StoreImpl.state) => {
    /* TODO: no-op */
    include StoreImpl;
};
