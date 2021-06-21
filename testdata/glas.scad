$fn=100;

cylinder(2, 15, 15, false);
cylinder(31, 3, 3, false);
translate([0, 0, 30]) difference() {
    cylinder(15, 3, 22, false);
    cylinder(15, 1, 20, false);
}
translate([9, -2.5, 0]) cube([5, 5, 5]);