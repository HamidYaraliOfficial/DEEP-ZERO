# DEEP ZERO — TDD

C++ owns simulation state; Blueprint is reserved for composition, animation and UI binding. GameInstance subsystems manage campaign services; WorldSubsystems manage world services; ActorComponents manage localized systems.

The architecture is suitable for offline play now and can be extended to server-authoritative multiplayer by replicating validated simulation snapshots rather than UI state.
