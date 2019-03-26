open Redux_Types;

/*
 * Make
 *
 * Functor that creates a Redux store based on a Model
 */
module Make: (ModelImpl: Model) => Store with
       type state = ModelImpl.state and
       type action = ModelImpl.action;

/*
 * Enhance
 *
 * A functor that acts as a 'store enhancer', allowing
 * a hook to override default functionality for methods.
 * This enables things like middleware and async stores
 */
module Enhance: (StoreImpl: Store, EnhancerImpl: StoreEnhancer with type action = StoreImpl.action and type state = StoreImpl.state) => Store with
    type state = StoreImpl.state and
    type action = StoreImpl.action;
