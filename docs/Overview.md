# Overview

This document is just a very quick introduction to the main components of the game engine, and how it's architected.

## Entity Component System

The Entity Component system implemented is similar to that of Unity. This means that it is not a true "ECS", but still somewhat similar (Components and Systems are combined here).

### Entities

Entities are essentially just containers that hold components and also contain some very basic information themselves (things like the Scene they belong to, their name, ID). They also have capabilities to allow sending events to the Entity, and then dispatching them to the components on that entity.

### Components
