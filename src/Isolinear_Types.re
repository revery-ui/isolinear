type subscriber('a, 'b) = ('a, 'b) => unit;
type unsubscribe = unit => unit;

module Effect = {
    type effectFunction;
    type t('a) = {
        name: string,
        f: unit => option('a),
    };

    let none: t('a) = {
       name: "None",
       f: () => None,
    };

    let run = (effect: t('a)) => {
        let _ = effect.f();   
    };

    /* let batch: (~name: string, List(t)) => Effect.t; */

    /* let create: (~name: string, ~f:effectFunction, ()) => Effect.t; */
};


module type Store = {
    type state;
    type actions;

    let getState: unit => state;

    let poll: unit => unit;

    let subscribe: subscriber(state, Effect.t(actions)) => unsubscribe;

    let dispatch: actions => unit;
};

module type Model = {
    type state;
    type actions;

    let initialState: state;
    let updater: (state, actions) => (state, Effect.t(actions));
};
