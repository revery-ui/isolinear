open Redux_Types;

/*
 * Make
 *
 * Functor that creates a Redux store based on a Model
 */
module Make: (ModelImpl: Model) => Store with
       type state = ModelImpl.state and
       type action = ModelImpl.action;

