#pragma once

void GenerateBoxCollisionMesh( CEtMeshStream &MeshStream, SCollisionBox &Box, DWORD dwColor = 0xffffffff );
void GenerateCapsuleCollisionMesh( CEtMeshStream &MeshStream, SCollisionCapsule &Capsule, DWORD dwColor = 0xffffffff );
void GenerateSphereCollisionMesh( CEtMeshStream &MeshStream, SCollisionSphere &Sphere, DWORD dwColor = 0xffffffff );
void GenerateTriangleListCollisionMesh( CEtMeshStream &MeshStream, SCollisionTriangleList &TriangleList, EtMatrix &WorldMat, DWORD dwColor = 0xffffffff );
