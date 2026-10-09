# Copyright Renato / LearnUnreal_56_1. All Rights Reserved.
# Flappy Bird in Unreal Verse (UEFN / Verse Language)
# Demonstrates Verse async concurrency, failable logic (<decides>), transactions (<transacts>),
# creative device bindings, and center-pivot geometry alignment.

using { /Fortnite.com/Devices }
using { /Verse.org/Simulation }
using { /UnrealEngine.com/Temporary/Diagnostics }
using { /UnrealEngine.com/Temporary/SpatialMath }
using { /UnrealEngine.com/Temporary/UI }
using { /Fortnite.com/UI }

# Pipe obstacle data structure in Verse
pipe_pair := struct<concrete>:
    X : float
    GapCenterZ : float
    Scored : logic

# Game states represented as a class hierarchy in Verse
flappy_game_state := enum:
    Ready
    Playing
    GameOver

# Creative Device controlling Flappy Bird game simulation
flappy_bird_device := class(creative_device):

    # Editable device references exposed to UEFN details panel
    @editable
    FlapTriggerButton : button_device = button_device{}

    @editable
    RestartTriggerButton : button_device = button_device{}

    # Physics configuration constants (centimeters, Z-Up coordinate space)
    FlapStrength : float = 480.0
    Gravity : float = 1200.0
    FloorZ : float = -450.0
    CeilingZ : float = 550.0

    # Obstacle dimensions
    PipeWidth : float = 100.0
    PipeSpeed : float = 320.0
    GapSize : float = 260.0
    PipeSpawnDistance : float = 800.0
    PipeDestroyDistance : float = -800.0
    SpawnIntervalSeconds : float = 1.75

    # Mutable state variables
    var CurrentState : flappy_game_state = flappy_game_state.Ready
    var BirdZ : float = 0.0
    var VerticalVelocity : float = 0.0
    var Score : int = 0
    var HighScore : int = 0
    var ActivePipes : []pipe_pair = array{}
    var TimeSinceLastSpawn : float = 0.0

    # Runs when the device initializes in the world
    OnBegin<override>()<suspends> : void =
        Print("Verse Flappy Bird Device Initialized.")
        
        # Subscribe to button trigger events
        FlapTriggerButton.InteractedWithEvent.Subscribe(OnFlapButtonPressed)
        RestartTriggerButton.InteractedWithEvent.Subscribe(OnRestartButtonPressed)

        ResetGame()
        
        # Launch async simulation loop
        spawn:
            GameLoop()

    # Event handler for flap input
    OnFlapButtonPressed(Agent : agent) : void =
        case (CurrentState):
            flappy_game_state.Ready =>
                set CurrentState = flappy_game_state.Playing
                Flap()
            flappy_game_state.Playing =>
                Flap()
            flappy_game_state.GameOver =>
                # Ignore flap until reset
                nop

    # Event handler for restart input
    OnRestartButtonPressed(Agent : agent) : void =
        if (CurrentState = flappy_game_state.GameOver):
            ResetGame()

    # Apply upward impulse in Verse
    Flap()<transacts> : void =
        if (CurrentState = flappy_game_state.Playing):
            set VerticalVelocity = FlapStrength

    # Reset game state
    ResetGame()<transacts> : void =
        set CurrentState = flappy_game_state.Ready
        set BirdZ = 0.0
        set VerticalVelocity = 0.0
        set Score = 0
        set ActivePipes = array{}
        set TimeSinceLastSpawn = 0.0
        Print("Game Ready. Press Flap to Begin!")

    # Bird crash handler
    Die()<transacts> : void =
        set CurrentState = flappy_game_state.GameOver
        if (Score > HighScore):
            set HighScore = Score
        Print("Bird Crashed! Final Score: {Score}. High Score: {HighScore}")

    # Core async game loop running at ~30Hz (Sleep(0.033))
    GameLoop()<suspends> : void =
        DeltaTime : float = 0.033
        loop:
            Sleep(DeltaTime)

            case (CurrentState):
                flappy_game_state.Ready =>
                    # Gentle idle hover
                    set BirdZ = 0.0

                flappy_game_state.Playing =>
                    UpdatePlaying(DeltaTime)

                flappy_game_state.GameOver =>
                    # Descend to floor if mid-air
                    if (BirdZ > FloorZ):
                        set VerticalVelocity = VerticalVelocity - (Gravity * DeltaTime)
                        set BirdZ = BirdZ + (VerticalVelocity * DeltaTime)
                        if (BirdZ < FloorZ):
                            set BirdZ = FloorZ

    # Physics & obstacle update step
    UpdatePlaying(DeltaTime : float)<transacts> : void =
        # 1. Integrate Gravity & Velocity
        set VerticalVelocity = VerticalVelocity - (Gravity * DeltaTime)
        set BirdZ = BirdZ + (VerticalVelocity * DeltaTime)

        # Check ceiling
        if (BirdZ > CeilingZ):
            set BirdZ = CeilingZ
            set VerticalVelocity = 0.0

        # Check floor
        if (BirdZ <= FloorZ):
            set BirdZ = FloorZ
            Die()
            return

        # 2. Spawn Pipes Timer
        set TimeSinceLastSpawn = TimeSinceLastSpawn + DeltaTime
        if (TimeSinceLastSpawn >= SpawnIntervalSeconds):
            set TimeSinceLastSpawn = 0.0
            SpawnPipe()

        # 3. Move Pipes & Check Collisions
        BirdRadius : float = 24.0
        BirdLeft : float = -BirdRadius
        BirdRight : float = BirdRadius
        BirdTop : float = BirdZ + BirdRadius
        BirdBottom : float = BirdZ - BirdRadius

        var UpdatedPipes : []pipe_pair = array{}

        for (Pipe : ActivePipes):
            NewX : float = Pipe.X - (PipeSpeed * DeltaTime)

            # Center-Pivot Gap Boundaries
            HalfGap : float = GapSize * 0.5
            TopPipeBottom : float = Pipe.GapCenterZ + HalfGap
            BottomPipeTop : float = Pipe.GapCenterZ - HalfGap

            var Scored : logic = Pipe.Scored

            # Score gate detection (passing bird X = 0)
            if (Scored = false and NewX < 0.0):
                set Scored = true
                set Score = Score + 1
                Print("Score: {Score}")

            # Collision detection (AABB)
            PipeLeft : float = NewX - (PipeWidth * 0.5)
            PipeRight : float = NewX + (PipeWidth * 0.5)

            if (BirdRight > PipeLeft and BirdLeft < PipeRight):
                if (BirdTop > TopPipeBottom or BirdBottom < BottomPipeTop):
                    Die()
                    return

            # Keep pipe if on-screen
            if (NewX > PipeDestroyDistance):
                NewPipe := pipe_pair{X := NewX, GapCenterZ := Pipe.GapCenterZ, Scored := Scored}
                set UpdatedPipes = UpdatedPipes + array{NewPipe}

        set ActivePipes = UpdatedPipes

    # Procedural pipe generation with pseudo-random gap offset
    SpawnPipe()<transacts> : void =
        # In Verse, pseudo-randomness or procedural seed can cycle gap heights
        PseudoRandomSeed : float = (Score * 73.0) - 120.0
        # Clamp gap center between -180.0 and +220.0
        var ClampedGap : float = PseudoRandomSeed
        if (ClampedGap < -180.0):
            set ClampedGap = -180.0
        if (ClampedGap > 220.0):
            set ClampedGap = 220.0

        NewPipe := pipe_pair{
            X := PipeSpawnDistance,
            GapCenterZ := ClampedGap,
            Scored := false
        }
        set ActivePipes = ActivePipes + array{NewPipe}
