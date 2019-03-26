type subscriber('a) = 'a => unit;
type unsubscribe = unit => unit;

module type Store = {
    type state;
    type action;

    let getState: unit => state;

    let poll: unit => unit;

    let subscribe: subscriber(state) => unsubscribe;

    let dispatch: action => unit;
};

module type StoreEnhancer  = {
    type state;
    type action;

    type store = {
        getState: unit => state,
        subscribe: subscriber(state) => unsubscribe,
        dispatch: action => unit,
    };

    type storeCreator = unit => store;

    let createStore: storeCreator => store;
};

module type Model = {
    type state;
    type action;

    let initialState: state;
    let reducer: (state, action) => state;
};
