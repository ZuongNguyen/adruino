---
name: arduino-wiring-check
description: Review or plan Arduino and low-voltage breadboard wiring from schematics, photos, Tinkercad circuits or netlists. Use for lap mach, dau day, kiem tra mach, LED/button/sensor/motor connections, polarity, power, GPIO limits and electrical troubleshooting. For behavioral logic and code-to-wiring agreement, also use circuit-logic-review.
---

# Arduino Wiring Check

Help the user assemble a circuit and explain whether the available wiring evidence supports operation. Respond in Vietnamese unless the user requests another language. Scale the review to the circuit; a simple LED exercise does not need a full engineering report.

## Establish the actual connections

- Identify the board and revision, component markings, supply rails and available sketch. Do not apply Uno R3 assumptions to Uno R4, ESP32 or another board merely because the connectors look similar.
- Inspect the supplied image or native schematic. Follow wires and repeated net labels end to end; a crossing is not automatically a junction. Record uncertain crossings or unreadable values rather than inventing a connection.
- For dense parallel wires, confirm disputed routes from both ends: GPIO to load and load back to GPIO. Use visible elbows, junction dots and endpoint labels as cross-checks. If the two traces disagree, resolve that discrepancy or mark the exact pin uncertain before recommending a new pin array.
- Keep component labels separate from Arduino pin names: LED D1 is not necessarily Arduino digital pin 1. Read anode/cathode symbols, not the left/right orientation of the drawing. A series resistor can be on either side of an LED or connected through a remote net label.
- On a breadboard, check connected row groups, the center gap, split power rails and the paired terminals of four-leg pushbuttons. A schematic cannot establish that a physical breadboard follows it.
- Infer a compact connection list when useful: source pin, component terminal, resistor/driver, destination rail. Preserve the user's requested circuit and identify the smallest concrete correction.

## Review the electrical path

- Trace each source-to-load-to-return path. Check supply polarity and voltage, shorts, missing returns and unintended direct connections between outputs. Non-isolated signals normally need a common reference; preserve intentional isolation.
- Check logic voltage compatibility, input thresholds, pull-up rail and source/sink current using the exact board/component documentation. Recommended operating conditions and absolute maximum ratings serve different purposes; absolute maxima are not design targets. Account for pin-group, MCU, rail and regulator limits when relevant.
- For LEDs, verify polarity and a current-limiting element in every independently driven branch. Estimate current with `(Vdrive - Vf) / R`, allowing for output voltage and LED variation; check resistor dissipation if material. A 1 kOhm resistor is not a fault just because an example uses 330 Ohm.
- For buttons, trace the input in both released and pressed states. External pull-down plus a switch to VCC normally means `INPUT`, pressed HIGH. A switch to GND with `INPUT_PULLUP` means pressed LOW. Check that the physical terminal pairs actually switch the input and that no state leaves it floating or shorts a rail.
- For motors, solenoids and relays, distinguish the GPIO control signal from load power. Check driver topology, startup/stall current, inductive suppression, supply decoupling and reset/off behavior. Servo signal pins and servo power are separate paths; do not demand a PWM-capable pin for every servo library.
- When a question depends on ratings or pin-specific behavior, consult the manufacturer's board schematic or datasheet and cite the relevant source. Do not treat a generic voltage range from a tutorial as the device's VIH/VIL specification.

## Diagnose without overstating evidence

For a circuit that misbehaves, choose the next observation that distinguishes plausible causes: supply at the board and load, ground return, input voltage in both switch states, output state, then one branch at a time. A failure after adding a part may be due to power or bus interactions, not necessarily a faulty part.

For physical measurement guidance, specify the meter setting, probe points and expected result. Continuity/resistance tests require power removed; current is measured in series, not by bridging a supply. Do not represent an unperformed measurement as evidence.

Complete useful static review even when physical measurements are unavailable. Say whether findings come from a schematic/photo, simulator, compilation, upload or actual hardware measurements. Compilation does not verify wiring; a correct schematic does not prove the assembled circuit works. Ask only for missing details that materially change the diagnosis.

Lead with the supported verdict, then concrete findings and fixes using component labels and pin numbers. Separate a confirmed fault from an unresolved detail or optional improvement. For an assembly request, provide an ordered wiring procedure with power disconnected while rewiring and a practical first check. For a review-only request, do not rewrite the sketch or diagram unless requested.

## Primary references

Use only the references needed by the actual circuit; follow the exact board's datasheet links when numerical limits matter.

- [Arduino digital pins](https://docs.arduino.cc/learn/microcontrollers/digital-pins/)
- [Arduino button with external pull-down](https://docs.arduino.cc/built-in-examples/digital/Button/)
- [Arduino internal pull-up example](https://docs.arduino.cc/built-in-examples/digital/InputPullupSerial/)
- [Uno R3 hardware and documentation](https://docs.arduino.cc/hardware/uno-rev3/)
