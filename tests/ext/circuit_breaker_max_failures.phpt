--TEST--
Test circuit_breaker functionality
--FILE--
<?php
function print_dd_tracer_circuit_breaker_is_closed(){
    echo (dd_tracer_circuit_breaker_is_closed() ? 'true' : 'false') . PHP_EOL;
}

dd_tracer_circuit_breaker_register_success();
dd_tracer_circuit_breaker_close();

print_dd_tracer_circuit_breaker_is_closed(); //> true
dd_tracer_circuit_breaker_register_error();
print_dd_tracer_circuit_breaker_is_closed(); //> true

dd_tracer_circuit_breaker_register_error();
print_dd_tracer_circuit_breaker_is_closed(); //> true

dd_tracer_circuit_breaker_register_error(); // 3th failure should trip the breaker
print_dd_tracer_circuit_breaker_is_closed(); //> false

// check if temporary closed breaker will reopen on first failure
dd_tracer_circuit_breaker_close();
print_dd_tracer_circuit_breaker_is_closed(); //> true

dd_tracer_circuit_breaker_register_error();
print_dd_tracer_circuit_breaker_is_closed(); //> false

// register success should reset the failure count
dd_tracer_circuit_breaker_register_success();
print_dd_tracer_circuit_breaker_is_closed(); //> true

dd_tracer_circuit_breaker_register_error();
print_dd_tracer_circuit_breaker_is_closed(); //> true

dd_tracer_circuit_breaker_register_error();
print_dd_tracer_circuit_breaker_is_closed(); //> true

dd_tracer_circuit_breaker_register_error(); // 3th failure should trip the breaker
print_dd_tracer_circuit_breaker_is_closed(); //> false

?>
--EXPECT--
true
true
true
false
true
false
true
true
true
false
