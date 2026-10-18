#pragma once

#include <memory>
#include <vector>

#include "raylib.h"

// ====================================================================
// PHYSIK-GRUNDKOERPER (Port von referenz/lib/phys.go)
//
// Ein Shape ist ein Koerper mit Masse/Traegheit, Position/Geschwindigkeit
// und Rotation. Es gibt zwei konkrete Formen: Polygon (konvex, aus
// Eckpunkten) und Circle (Kreis mit Ausrichtungslinie).
//
// Shape ist eine abstrakte Basisklasse: Das Go-Interface Shape wird 1:1
// durch virtuelle Methoden abgebildet, die gemeinsamen Bewegungsgroessen
// (BasicShape) liegen in der Basis. Koerper werden ueber
// CreatePolygon/CreateCircle als std::unique_ptr<Shape> erzeugt und
// uebergeben.
//
// Definition: shape.cpp
// ====================================================================

// BasicShape buendelt die gemeinsamen Bewegungsgroessen.
struct BasicShape {
    Vector2 Location{};             // Schwerpunkt/Schwerpunktposition
    Vector2 Velocity{};             // Geschwindigkeit
    float   AngVelocity = 0.0f;     // Winkelgeschwindigkeit
    Vector2 Accel{};                // Beschleunigung (pro Update zurueckgesetzt)
    float   AngAccel = 0.0f;        // Winkelbeschleunigung
    float   Mass = 0.0f;            // Masse (MaxFloat = unendlich/wall)
    float   Inertia = 0.0f;         // Traegheitsmoment
    bool    IsGrounded = false;     // liegt auf einer Unterlage auf
};

// Shape ist die abstrakte Basisklasse aller Koerper. Fuer beide Formen
// identische Operationen (ApplyForce, ClearState) werden einmalig hier
// implementiert, die Form-spezifischen sind pure virtuelle Methoden.
struct Shape {
    BasicShape Basic;

    virtual ~Shape() = default;
    virtual void Update() = 0;
    virtual void Draw(Color color, float thickness) const = 0;
    virtual void Rotate(float angle) = 0;
    virtual void ApplyGravity(Vector2 gravity) = 0;
    virtual void ResetPos(Vector2 delta) = 0;

    // Fuer Polygon und Circle identisch.
    void ApplyForce(Vector2 force, float angForce);
    void ClearState();
};

// Polygon: konvexe Form aus Eckpunkten (in Weltkoordinaten).
struct Polygon : public Shape {
    std::vector<Vector2> Vertices;

    void Update() override;
    void Draw(Color color, float thickness) const override;
    void Rotate(float angle) override;
    void ApplyGravity(Vector2 gravity) override;
    void ResetPos(Vector2 delta) override;
};

// Circle: Kreis mit Radius; Orientation ist ein Punkt auf dem Rand und
// dient als sichtbare Rotationsmarkierung.
struct Circle : public Shape {
    float   Radius = 0.0f;
    Vector2 Orientation{};

    void Update() override;
    void Draw(Color color, float thickness) const override;
    void Rotate(float angle) override;
    void ApplyGravity(Vector2 gravity) override;
    void ResetPos(Vector2 delta) override;
};

// CreatePolygon erzeugt ein achsenparalleles Rechteck als Polygon.
// wall == true -> unendliche Masse/Traegheit (beweglicher Koerper
// bleiben unbeeinflusst).
std::unique_ptr<Shape> CreatePolygon(float x, float y, float w, float h, bool wall);

// CreateCircle erzeugt einen Kreis. wall == true -> unendliche Masse.
std::unique_ptr<Shape> CreateCircle(float x, float y, float r, bool wall);
