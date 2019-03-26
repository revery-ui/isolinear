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

/* module type Effect = { */
/*     type t('a) = { */
/*         name: string, */
/*         f: unit => option('a), */
/*     } */

/*     let none = { */
/*         name: "None", */
/*         f: () => None, */
/*     }; */

/*     let batch = (~name="Batched Effects", effects: list(Effect.) */

/*     let create = (~name, ~f) => { */
/*         name, */
/*         f, */
/*     }; */
/* }; */

module type Model = {
    type state;
    type action;

    let initialState: state;
    let updater: (state, action) => (state, Effect.t(action));
};
