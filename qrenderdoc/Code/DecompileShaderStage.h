/** Copyright (c) 2019-2021 Paper Games */

#pragma once
#include <vector>
#include "Code/Interface/QRDInterface.h"

enum class DecompileShaderTemporalFileType
{
  None,
  ResourceHlsl,
  ResourceDxbcReadable,
  SourceDxbcReadable,
  TempGLSL,
  DecompiledHlsl,
  DecompiledDxbcReadable
};

// File Name:
// Resource     HLSL:       resource_hlsl
// Resource     DXBC:       resource_dxbc
// Source       DXBC:       source_dxbc
// Decompiled   HLSL:       decompiled_hlsl
// Decompiled   DXBC:       decompiled_dxbc
static QMap<DecompileShaderTemporalFileType, std::pair<rdcstr, rdcstr>> 
GDecompileShaderStageFileTypeMap
= {
  {DecompileShaderTemporalFileType::ResourceHlsl, {lit("resource_hlsl"), lit(".hlsl")}},
  {DecompileShaderTemporalFileType::ResourceDxbcReadable, {lit("resource_dxbc"), lit(".dxbc")}},
  // {DecompileShaderTemporalFileType::SourceDxbcBinary, {lit("source_dxbc"), lit(".dxbc.bin")}},
  {DecompileShaderTemporalFileType::SourceDxbcReadable, {lit("source_dxbc"), lit(".dxbc")}},
  {DecompileShaderTemporalFileType::TempGLSL, {lit("combined"), lit(".glsl")}},
  {DecompileShaderTemporalFileType::DecompiledHlsl, {lit("decompiled_hlsl"), lit(".hlsl")}},
  {DecompileShaderTemporalFileType::DecompiledDxbcReadable, {lit("decompiled_dxbc"), lit(".dxbc")}}};
