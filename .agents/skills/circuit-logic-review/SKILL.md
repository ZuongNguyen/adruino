---
name: circuit-logic-review
description: Check circuit behavior, digital logic and agreement between Arduino wiring and firmware. Use for check logic mach, mach co chay khong, wrong LED patterns, inverted buttons, pin mapping, truth tables, debounce, edge detection, state transitions and timing. Electrical ratings and assembly paths belong to arduino-wiring-check when relevant.
---

# Circuit Logic Review

Determine whether the observed circuit and program implement the requested behavior. Explain in Vietnamese unless asked otherwise. Review the artifacts the user actually supplied; do not assume another exercise's pin assignment or input polarity.

## Reconcile behavior, wiring and code

1. Establish the requested behavior and what is available: schematic/photo, logic-gate diagram, current `.ino` or project source, simulator trace or measured observations. If code is absent, review what the wiring establishes and keep firmware compatibility conditional.
2. Derive the pin map from the current circuit independently of the program. Match that map against `pinMode`, pin arrays, aliases and writes/reads in the actual source. Component D1 and Arduino pin D1 are different identifiers. Trace remote net labels before declaring a pin disconnected.
   Confirm ambiguous paths from both the GPIO and component endpoints before prescribing an exact replacement pin map. A crowded image can support the general finding of a mismatch while leaving an individual route unresolved.
3. Derive active levels from current paths: released/pressed input values, active HIGH or LOW LEDs, transistor/relay inversions and pull-up/down arrangement. State the relationship between a logic value and a physical action.
4. For combinational circuits, derive the relevant Boolean expression or truth cases, including inverted pins and enable inputs. For sequential or programmed circuits, follow reset/initial state and state transitions. Use a compact table or timing sketch only when it helps expose an error.
5. Compare expected and actual outputs for representative events. Include idle/startup, press and release, holding a button, repeated presses and any simultaneous-input behavior required by the task. Do not invent a priority rule or a required feature the user did not request.

## Arduino-specific checks

- Confirm `INPUT` versus `INPUT_PULLUP` matches the external circuit and the pressed-state comparison. Review both raw and debounced initial values; initialization must agree with the intended startup behavior, or be sampled appropriately from the input.
- Distinguish a level-triggered action from a one-time press edge. Holding a button should not repeatedly toggle state if the requirement is one toggle per press. Debounce applies to transitions; examine press, release and a second press, not just the first event.
- Verify LED numbering, array order, loop bounds, even/odd rules and initial output values. Different subprojects may deliberately use opposite pin orders.
- Inspect blocking delays only where responsiveness or timing requirements make them relevant. For `millis()` timing, use unsigned elapsed-time subtraction so rollover does not break a deadline. Review interrupt/PWM/timer conflicts only when those peripherals are used.
- Check whether outputs are initialized before enabling a load and what the external pull resistors enforce during reset. Do not infer a safe hardware state only from the first line executed in `setup()`.

## Verification proportional to the claim

- A static trace can demonstrate a mapping or logic mismatch. Give the triggering input and the resulting wrong state, followed by the smallest fix.
- For an authorized code change, use the installed Arduino CLI or established project toolchain where practical. Identify the target board/core rather than blindly using `arduino:avr:uno` for every Uno-like board. Record the command and whether compilation succeeded. Check library needs before installing anything.
- When simulation is available and useful, exercise the relevant input sequence and inspect actual outputs. Do not call a manual mental trace a simulator run. If no simulator is used, say so.
- Compilation proves build compatibility only; uploading proves the transfer only. Neither verifies electrical correctness or actual hardware behavior. Do not upload a sketch merely to review it.
- Hardware tests should name the input action, expected pin/LED state and observation needed. Stop short of asserting a measured result when the user has not supplied it.

If asked only to review, explain corrections without altering files. When changes are authorized, preserve working examples and keep code and wiring instructions consistent with the chosen circuit. Do not silently switch between pull-up and pull-down designs.

Lead with whether the circuit can operate and whether the available code matches it. Separate definite errors, conditional compatibility and missing evidence. Keep routine reviews short; include code or a detailed trace when needed to make the fix usable.

## Primary references

- [Arduino pin modes](https://docs.arduino.cc/language-reference/en/variables/constants/inputOutputPullup/)
- [Arduino debounce example](https://docs.arduino.cc/built-in-examples/digital/Debounce/)
- [Arduino state change detection](https://docs.arduino.cc/built-in-examples/digital/StateChangeDetection/)
- [Arduino millis reference](https://docs.arduino.cc/language-reference/en/functions/time/millis/)
