open Isolinear_Types;

/*
 * Make
 *
 * Functor that creates an Isolinear store based on a Model
 */
module Make: (ModelImpl: Model) => Store with
       type state = ModelImpl.state and
       type actions = ModelImpl.actions;

