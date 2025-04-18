// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

UENUM(meta = (Bitflags))
enum class EObjectMask : uint8
{
    ObstacleMask = 1 << 0,
    SubObstacleMask = 1 << 1,
    FenceMask = 1 << 2,
    ResourceMask = 1 << 3,
    DecoMask = 1 << 4
};

UENUM()
enum class EPatternType : uint8
{
    Rectangle,
    UShape,
    LShape
};