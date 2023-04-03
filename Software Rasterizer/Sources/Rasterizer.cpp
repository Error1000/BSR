#include "..\Headers\Main.hpp"



const Math::Mat4f Rasterizer::Camera::GetViewMatrix() const
{
	return
		Math::Mat4f::GetRotation(AngleTilt, Math::Vec3f(0.0f, 0.0f, 1.0f)) *
		Math::Mat4f::GetRotation(AngleVertical, Math::Vec3f(1.0f, 0.0f, 0.0f)) *
		Math::Mat4f::GetRotation(AngleFlat, Math::Vec3f(0.0f, 1.0f, 0.0f)) *
		Math::Mat4f::GetTranslation(-Position);
}

const Math::Mat4f Rasterizer::Camera::GetProjectionMatrix(const float _AspectRatio) const
{
	if (Perspective)
	{
		return Math::Mat4f::GetPerspective(FieldOfView, _AspectRatio, NearPlane, FarPlane);
	}

	return Math::Mat4f::GetOrtho(-FieldOfView / 2.0f * _AspectRatio, FieldOfView / 2.0f * _AspectRatio, -FieldOfView / 2.0f, FieldOfView / 2.0f, -FarPlane, -NearPlane);
}



Rasterizer::VertexBuffer::VertexBuffer() : Verteces()
{

}

Rasterizer::VertexBuffer::VertexBuffer(const VertexBuffer& _Other) : Verteces(_Other.Verteces)
{

}

Rasterizer::VertexBuffer::VertexBuffer(VertexBuffer&& _Other) noexcept : Verteces(std::move(_Other.Verteces))
{

}

Rasterizer::VertexBuffer::~VertexBuffer()
{

}

void Rasterizer::VertexBuffer::PushBack(const VertexData& _Vertex)
{
	Verteces.PushBack(_Vertex);
}

void Rasterizer::VertexBuffer::Erase(const size_t _Index)
{
	Verteces.Erase(_Index);
}

void Rasterizer::VertexBuffer::Clear()
{
	Verteces.Clear();
}

const size_t Rasterizer::VertexBuffer::GetSize() const
{
	return Verteces.GetSize();
}

Rasterizer::VertexData& Rasterizer::VertexBuffer::operator[] (const size_t _Index)
{
	return Verteces[_Index];
}

const Rasterizer::VertexData& Rasterizer::VertexBuffer::operator[] (const size_t _Index) const
{
	return Verteces[_Index];
}

void Rasterizer::VertexBuffer::operator= (const VertexBuffer& _Other)
{
	Verteces = _Other.Verteces;
}

void Rasterizer::VertexBuffer::operator= (VertexBuffer&& _Other) noexcept
{
	Verteces = std::move(_Other.Verteces);
}



Rasterizer::IndexBuffer::IndexBuffer() : Indexes()
{

}

Rasterizer::IndexBuffer::IndexBuffer(const IndexBuffer& _Other) : Indexes(_Other.Indexes)
{

}

Rasterizer::IndexBuffer::IndexBuffer(IndexBuffer&& _Other) noexcept : Indexes(std::move(_Other.Indexes))
{

}

Rasterizer::IndexBuffer::~IndexBuffer()
{

}

void Rasterizer::IndexBuffer::PushBack(const IndexData& _Index)
{
	Indexes.PushBack(_Index);
}

void Rasterizer::IndexBuffer::Erase(const size_t _Index)
{
	Indexes.Erase(_Index);
}

void Rasterizer::IndexBuffer::Clear()
{
	Indexes.Clear();
}

const size_t Rasterizer::IndexBuffer::GetSize() const
{
	return Indexes.GetSize();
}

Rasterizer::IndexData& Rasterizer::IndexBuffer::operator[] (const size_t _Index)
{
	return Indexes[_Index];
}

const Rasterizer::IndexData& Rasterizer::IndexBuffer::operator[] (const size_t _Index) const
{
	return Indexes[_Index];
}

void Rasterizer::IndexBuffer::operator= (const IndexBuffer& _Other)
{
	Indexes = _Other.Indexes;
}

void Rasterizer::IndexBuffer::operator= (IndexBuffer&& _Other) noexcept
{
	Indexes = std::move(_Other.Indexes);
}



Rasterizer::Model::Model() : Meshes()
{

}

Rasterizer::Model::Model(const Model& _Other) : Meshes(_Other.Meshes)
{

}

Rasterizer::Model::Model(Model&& _Other) noexcept : Meshes(std::move(_Other.Meshes))
{

}

Rasterizer::Model::~Model()
{
	for (size_t _Index = 0; _Index < Meshes.GetSize(); _Index++)
	{
		delete[] Meshes[_Index].Name;
	}
}

bool Rasterizer::Model::Load(const wchar_t* _Path)
{
	struct MeshFileData
	{
		wchar_t* Name = nullptr;
		size_t FacesStart = (size_t)(-1);
		size_t FacesEnd = (size_t)(-1);
	};

	struct FaceVertex
	{
		size_t Position = (size_t)(-1);
		size_t Normal = (size_t)(-1);
		size_t TextureCoords = (size_t)(-1);
	};

	Meshes.Clear();

	if (!_Path)
	{
		return false;
	}

	std::wifstream _fIn;

	_fIn.open(_Path);

	if (!_fIn.is_open())
	{
		return false;
	}

	Vector<MeshFileData> _Meshes;

	Vector<Math::Vec3f> _Positions;
	Vector<Math::Vec3f> _Normals;
	Vector<Math::Vec2f> _TextureCoords;

	Vector<Vector<FaceVertex>> _Faces;

	wchar_t _Line[OBJ_MAX_LINE_LEN + 1];
	wchar_t _OriginalLine[OBJ_MAX_LINE_LEN + 1];

	while (!_fIn.eof())
	{
		_fIn.getline(_OriginalLine, OBJ_MAX_LINE_LEN);

		for (size_t _Index = 0; _Index < OBJ_MAX_LINE_LEN + 1; _Index++)
		{
			_Line[_Index] = _OriginalLine[_Index];
		}

		Vector<const wchar_t*> _Tokens;

		{
			const wchar_t* _Token = String::TokenizeWhiteSpace(_Line);

			while (_Token)
			{
				_Tokens.PushBack(_Token);

				_Token = String::TokenizeWhiteSpace(nullptr);
			}
		}

		if (!_Tokens.GetSize())
		{
			continue;
		}

		if (_Tokens[0][0] == L'#')
		{
			continue;
		}

		if (String::TheSame(_Tokens[0], L"o"))
		{
			if (_Tokens.GetSize() != 2)
			{
				_fIn.close();
				for (size_t _IndexDelete = 0; _IndexDelete < _Meshes.GetSize(); _IndexDelete++)
				{
					delete[] _Meshes[_IndexDelete].Name;
				}
				return false;
			}

			for (size_t _Index = 0; _Index < _Meshes.GetSize(); _Index++)
			{
				if (String::TheSame(_Tokens[1], _Meshes[_Index].Name))
				{
					_fIn.close();
					for (size_t _IndexDelete = 0; _IndexDelete < _Meshes.GetSize(); _IndexDelete++)
					{
						delete[] _Meshes[_IndexDelete].Name;
					}
					return false;
				}
			}

			MeshFileData _MeshFileData;

			size_t _NameLen = String::Length(_Tokens[1]);

			_MeshFileData.Name = new wchar_t[_NameLen + 1];

			if (!_MeshFileData.Name)
			{
				_fIn.close();
				for (size_t _IndexDelete = 0; _IndexDelete < _Meshes.GetSize(); _IndexDelete++)
				{
					delete[] _Meshes[_IndexDelete].Name;
				}
				return false;
			}

			for (size_t _Index = 0; _Index < _NameLen + 1; _Index++)
			{
				_MeshFileData.Name[_Index] = _Tokens[1][_Index];
			}

			_MeshFileData.FacesStart = _Faces.GetSize();

			if (_Meshes.GetSize())
			{
				_Meshes[_Meshes.GetSize() - 1].FacesEnd = _Faces.GetSize();
			}

			_Meshes.PushBack(_MeshFileData);

			continue;
		}

		if (String::TheSame(_Tokens[0], L"v"))
		{
			if (_Tokens.GetSize() != 4)
			{
				_fIn.close();
				for (size_t _IndexDelete = 0; _IndexDelete < _Meshes.GetSize(); _IndexDelete++)
				{
					delete[] _Meshes[_IndexDelete].Name;
				}
				return false;
			}

			_Positions.PushBack(Math::Vec3f(std::stof(_Tokens[1]), std::stof(_Tokens[2]), std::stof(_Tokens[3])));

			continue;
		}

		if (String::TheSame(_Tokens[0], L"vn"))
		{
			if (_Tokens.GetSize() != 4)
			{
				_fIn.close();
				for (size_t _IndexDelete = 0; _IndexDelete < _Meshes.GetSize(); _IndexDelete++)
				{
					delete[] _Meshes[_IndexDelete].Name;
				}
				return false;
			}

			_Normals.PushBack((Math::Vec3f(std::stof(_Tokens[1]), std::stof(_Tokens[2]), std::stof(_Tokens[3]))));

			continue;
		}

		if (String::TheSame(_Tokens[0], L"vt"))
		{
			if (_Tokens.GetSize() != 3)
			{
				_fIn.close();
				for (size_t _IndexDelete = 0; _IndexDelete < _Meshes.GetSize(); _IndexDelete++)
				{
					delete[] _Meshes[_IndexDelete].Name;
				}
				return false;
			}

			_TextureCoords.PushBack((Math::Vec2f(std::stof(_Tokens[1]), std::stof(_Tokens[2]))));

			continue;
		}

		if (String::TheSame(_Tokens[0], L"f"))
		{
			if (_Tokens.GetSize() < 4)
			{
				_fIn.close();
				for (size_t _IndexDelete = 0; _IndexDelete < _Meshes.GetSize(); _IndexDelete++)
				{
					delete[] _Meshes[_IndexDelete].Name;
				}
				return false;
			}

			_Faces.PushBack(Vector<FaceVertex>());

			Vector<FaceVertex>& _CurrentFace = _Faces[_Faces.GetSize() - 1];

			for (size_t _Index = 1; _Index < _Tokens.GetSize(); _Index++)
			{
				size_t _TokenLen = String::Length(_Tokens[_Index]);

				if (_Tokens[_Index][0] == L'/' || _Tokens[_Index][_TokenLen - 1] == L'/')
				{
					_fIn.close();
					for (size_t _IndexDelete = 0; _IndexDelete < _Meshes.GetSize(); _IndexDelete++)
					{
						delete[] _Meshes[_IndexDelete].Name;
					}
					return false;
				}

				FaceVertex _FaceVertex;

				size_t _CountSlash = String::Count(_Tokens[_Index], L'/');

				switch (_CountSlash)
				{
				case 0:
				{
					X64_CALL(_FaceVertex.Position = std::stoull(_Tokens[_Index]) - 1);
					X86_CALL(_FaceVertex.Position = std::stoul(_Tokens[_Index]) - 1);

					if (_FaceVertex.Position >= _Positions.GetSize())
					{
						_fIn.close();
						for (size_t _IndexDelete = 0; _IndexDelete < _Meshes.GetSize(); _IndexDelete++)
						{
							delete[] _Meshes[_IndexDelete].Name;
						}
						return false;
					}

					break;
				}
				case 1:
				{
					wchar_t _FaceVertexText[OBJ_MAX_LINE_LEN + 1];

					for (size_t _IndexCopy = 0; _IndexCopy < _TokenLen + 1; _IndexCopy++)
					{
						_FaceVertexText[_IndexCopy] = _Tokens[_Index][_IndexCopy];
					}

					Vector<const wchar_t*> _FaceVertexTokens;

					{
						const wchar_t* _Token = String::TokenizeSlashes(_FaceVertexText);

						while (_Token)
						{
							_FaceVertexTokens.PushBack(_Token);

							_Token = String::TokenizeSlashes(nullptr);
						}
					}

					X64_CALL(_FaceVertex.Position = std::stoull(_FaceVertexTokens[0]) - 1);
					X86_CALL(_FaceVertex.Position = std::stoul(_FaceVertexTokens[0]) - 1);

					if (_FaceVertex.Position >= _Positions.GetSize())
					{
						_fIn.close();
						for (size_t _IndexDelete = 0; _IndexDelete < _Meshes.GetSize(); _IndexDelete++)
						{
							delete[] _Meshes[_IndexDelete].Name;
						}
						return false;
					}

					X64_CALL(_FaceVertex.TextureCoords = std::stoull(_FaceVertexTokens[1]) - 1);
					X86_CALL(_FaceVertex.TextureCoords = std::stoul(_FaceVertexTokens[1]) - 1);

					if (_FaceVertex.TextureCoords >= _TextureCoords.GetSize())
					{
						_fIn.close();
						for (size_t _IndexDelete = 0; _IndexDelete < _Meshes.GetSize(); _IndexDelete++)
						{
							delete[] _Meshes[_IndexDelete].Name;
						}
						return false;
					}

					break;
				}
				case 2:
				{
					wchar_t _FaceVertexText[OBJ_MAX_LINE_LEN + 1];

					for (size_t _IndexCopy = 0; _IndexCopy < _TokenLen + 1; _IndexCopy++)
					{
						_FaceVertexText[_IndexCopy] = _Tokens[_Index][_IndexCopy];
					}

					Vector<const wchar_t*> _FaceVertexTokens;

					{
						const wchar_t* _Token = String::TokenizeSlashes(_FaceVertexText);

						while (_Token)
						{
							_FaceVertexTokens.PushBack(_Token);

							_Token = String::TokenizeSlashes(nullptr);
						}
					}

					if (_FaceVertexTokens.GetSize() == 2)
					{
						X64_CALL(_FaceVertex.Position = std::stoull(_FaceVertexTokens[0]) - 1);
						X86_CALL(_FaceVertex.Position = std::stoul(_FaceVertexTokens[0]) - 1);

						if (_FaceVertex.Position >= _Positions.GetSize())
						{
							_fIn.close();
							for (size_t _IndexDelete = 0; _IndexDelete < _Meshes.GetSize(); _IndexDelete++)
							{
								delete[] _Meshes[_IndexDelete].Name;
							}
							return false;
						}

						X64_CALL(_FaceVertex.Normal = std::stoull(_FaceVertexTokens[1]) - 1);
						X86_CALL(_FaceVertex.Normal = std::stoul(_FaceVertexTokens[1]) - 1);

						if (_FaceVertex.Normal >= _Normals.GetSize())
						{
							_fIn.close();
							for (size_t _IndexDelete = 0; _IndexDelete < _Meshes.GetSize(); _IndexDelete++)
							{
								delete[] _Meshes[_IndexDelete].Name;
							}
							return false;
						}
					}
					else
					{
						X64_CALL(_FaceVertex.Position = std::stoull(_FaceVertexTokens[0]) - 1);
						X86_CALL(_FaceVertex.Position = std::stoul(_FaceVertexTokens[0]) - 1);

						if (_FaceVertex.Position >= _Positions.GetSize())
						{
							_fIn.close();
							for (size_t _IndexDelete = 0; _IndexDelete < _Meshes.GetSize(); _IndexDelete++)
							{
								delete[] _Meshes[_IndexDelete].Name;
							}
							return false;
						}

						X64_CALL(_FaceVertex.TextureCoords = std::stoull(_FaceVertexTokens[1]) - 1);
						X86_CALL(_FaceVertex.TextureCoords = std::stoul(_FaceVertexTokens[1]) - 1);

						if (_FaceVertex.TextureCoords >= _TextureCoords.GetSize())
						{
							_fIn.close();
							for (size_t _IndexDelete = 0; _IndexDelete < _Meshes.GetSize(); _IndexDelete++)
							{
								delete[] _Meshes[_IndexDelete].Name;
							}
							return false;
						}

						X64_CALL(_FaceVertex.Normal = std::stoull(_FaceVertexTokens[2]) - 1);
						X86_CALL(_FaceVertex.Normal = std::stoul(_FaceVertexTokens[2]) - 1);

						if (_FaceVertex.Normal >= _Normals.GetSize())
						{
							_fIn.close();
							for (size_t _IndexDelete = 0; _IndexDelete < _Meshes.GetSize(); _IndexDelete++)
							{
								delete[] _Meshes[_IndexDelete].Name;
							}
							return false;
						}
					}

					break;
				}
				default:
				{
					_fIn.close();
					for (size_t _IndexDelete = 0; _IndexDelete < _Meshes.GetSize(); _IndexDelete++)
					{
						delete[] _Meshes[_IndexDelete].Name;
					}
					return false;
				}
				}

				_CurrentFace.PushBack(_FaceVertex);
			}

			continue;
		}
	}

	if (!_Meshes.GetSize())
	{
		_fIn.close();
		return false;
	}

	_Meshes[_Meshes.GetSize() - 1].FacesEnd = _Faces.GetSize();

	for (size_t _IndexMesh = 0; _IndexMesh < _Meshes.GetSize(); _IndexMesh++)
	{
		MeshFileData& _CurrentMesh = _Meshes[_IndexMesh];

		Mesh _Mesh;

		_Mesh.Name = _CurrentMesh.Name;

		for (size_t _IndexFace = _CurrentMesh.FacesStart; _IndexFace < _CurrentMesh.FacesEnd; _IndexFace++)
		{
			Vector<FaceVertex>& _CurrentFace = _Faces[_IndexFace];

			for (size_t _IndexFaceVertex = 1; _IndexFaceVertex < _CurrentFace.GetSize() - 1; _IndexFaceVertex++)
			{
				VertexData _VertA;
				VertexData _VertB;
				VertexData _VertC;

				_VertA.Position = _Positions[_CurrentFace[0].Position];
				_VertA.Color = Math::Vec4f(1.0f, 1.0f, 1.0f, 1.0f);

				_VertB.Position = _Positions[_CurrentFace[_IndexFaceVertex].Position];
				_VertB.Color = Math::Vec4f(1.0f, 1.0f, 1.0f, 1.0f);

				_VertC.Position = _Positions[_CurrentFace[_IndexFaceVertex + 1].Position];
				_VertC.Color = Math::Vec4f(1.0f, 1.0f, 1.0f, 1.0f);

				if (_CurrentFace[0].Normal != (size_t)(-1))
				{
					_VertA.Normal = _Normals[_CurrentFace[0].Normal];
				}
				else
				{
					_VertA.Normal = Math::Vec3f::Cross(_VertB.Position - _VertA.Position, _VertC.Position - _VertA.Position);
				}
				if (_CurrentFace[0].TextureCoords != (size_t)(-1))
				{
					_VertA.TextureCoords = _TextureCoords[_CurrentFace[0].TextureCoords];
				}
				else
				{
					_VertA.TextureCoords = Math::Vec2f(0.0f, 0.0f);
				}

				if (_CurrentFace[_IndexFaceVertex].Normal != (size_t)(-1))
				{
					_VertB.Normal = _Normals[_CurrentFace[_IndexFaceVertex].Normal];
				}
				else
				{
					_VertB.Normal = Math::Vec3f::Cross(_VertB.Position - _VertA.Position, _VertC.Position - _VertA.Position);
				}
				if (_CurrentFace[_IndexFaceVertex].TextureCoords != (size_t)(-1))
				{
					_VertB.TextureCoords = _TextureCoords[_CurrentFace[_IndexFaceVertex].TextureCoords];
				}
				else
				{
					_VertB.TextureCoords = Math::Vec2f(0.0f, 0.0f);
				}

				if (_CurrentFace[_IndexFaceVertex + 1].Normal != (size_t)(-1))
				{
					_VertC.Normal = _Normals[_CurrentFace[_IndexFaceVertex + 1].Normal];
				}
				else
				{
					_VertC.Normal = Math::Vec3f::Cross(_VertB.Position - _VertA.Position, _VertC.Position - _VertA.Position);
				}
				if (_CurrentFace[_IndexFaceVertex + 1].TextureCoords != (size_t)(-1))
				{
					_VertC.TextureCoords = _TextureCoords[_CurrentFace[_IndexFaceVertex + 1].TextureCoords];
				}
				else
				{
					_VertC.TextureCoords = Math::Vec2f(0.0f, 0.0f);
				}

				if (_CurrentFace[0].TextureCoords != (size_t)(-1) && _CurrentFace[_IndexFaceVertex].TextureCoords != (size_t)(-1) && _CurrentFace[_IndexFaceVertex + 1].TextureCoords != (size_t)(-1))
				{
					if (_VertA.TextureCoords != _VertB.TextureCoords && _VertB.TextureCoords != _VertC.TextureCoords && _VertC.TextureCoords != _VertA.TextureCoords)
					{
						Math::Vec3f _Edge1 = _VertB.Position - _VertA.Position;
						Math::Vec3f _Edge2 = _VertC.Position - _VertA.Position;

						Math::Vec2f _Delta1 = _VertB.TextureCoords - _VertA.TextureCoords;
						Math::Vec2f _Delta2 = _VertC.TextureCoords - _VertA.TextureCoords;

						float _Factor = 1.0f / (_Delta1.x * _Delta2.y - _Delta2.x * _Delta1.y);

						_VertA.Tangent.x = _Factor * (_Delta2.y * _Edge1.x - _Delta1.y * _Edge2.x);
						_VertA.Tangent.y = _Factor * (_Delta2.y * _Edge1.y - _Delta1.y * _Edge2.y);
						_VertA.Tangent.z = _Factor * (_Delta2.y * _Edge1.z - _Delta1.y * _Edge2.z);

						_VertA.Tangent /= _VertA.Tangent.Magnitude();

						_VertB.Tangent.x = _VertA.Tangent.x;
						_VertB.Tangent.y = _VertA.Tangent.y;
						_VertB.Tangent.z = _VertA.Tangent.z;

						_VertC.Tangent.x = _VertA.Tangent.x;
						_VertC.Tangent.y = _VertA.Tangent.y;
						_VertC.Tangent.z = _VertA.Tangent.z;
					}
					else
					{
						_VertA.Tangent = Math::Vec3f(0.0f, 0.0f, 0.0f);
						_VertB.Tangent = Math::Vec3f(0.0f, 0.0f, 0.0f);
						_VertC.Tangent = Math::Vec3f(0.0f, 0.0f, 0.0f);
					}
				}
				else
				{
					_VertA.Tangent = Math::Vec3f(0.0f, 0.0f, 0.0f);
					_VertB.Tangent = Math::Vec3f(0.0f, 0.0f, 0.0f);
					_VertC.Tangent = Math::Vec3f(0.0f, 0.0f, 0.0f);
				}

				IndexData _IndexData;

				_IndexData.IndexA = _Mesh.VBO.GetSize();
				_IndexData.IndexB = _Mesh.VBO.GetSize() + 1;
				_IndexData.IndexC = _Mesh.VBO.GetSize() + 2;

				_Mesh.VBO.PushBack(_VertA);
				_Mesh.VBO.PushBack(_VertB);
				_Mesh.VBO.PushBack(_VertC);

				_Mesh.IBO.PushBack(_IndexData);
			}
		}

		Meshes.EmplaceBack(std::move(_Mesh));
	}

	_fIn.close();

	return true;
}

void Rasterizer::Model::PushBack(const Mesh& _Mesh)
{
	for (size_t _Index = 0; _Index < Meshes.GetSize(); _Index++)
	{
		if (String::TheSame(Meshes[_Index].Name, _Mesh.Name))
		{
			DEBUG_BREAK();
			return;
		}
	}

	Meshes.PushBack(_Mesh);
}

void Rasterizer::Model::EmplaceBack(Mesh && _Mesh) noexcept
{
	for (size_t _Index = 0; _Index < Meshes.GetSize(); _Index++)
	{
		if (String::TheSame(Meshes[_Index].Name, _Mesh.Name))
		{
			DEBUG_BREAK();
			return;
		}
	}

	Meshes.EmplaceBack(std::move(_Mesh));
}

void Rasterizer::Model::Erase(const size_t _Index)
{
	delete[] Meshes[_Index].Name;

	Meshes.Erase(_Index);
}

void Rasterizer::Model::Clear()
{
	for (size_t _Index = 0; _Index < Meshes.GetSize(); _Index++)
	{
		delete[] Meshes[_Index].Name;
	}

	Meshes.Clear();
}

const size_t Rasterizer::Model::GetSize() const
{
	return Meshes.GetSize();
}

Rasterizer::Mesh& Rasterizer::Model::operator[] (const size_t _Index)
{
	return Meshes[_Index];
}

const Rasterizer::Mesh& Rasterizer::Model::operator[] (const size_t _Index) const
{
	return Meshes[_Index];
}

void Rasterizer::Model::operator= (const Model& _Other)
{
	Meshes = _Other.Meshes;
}

void Rasterizer::Model::operator= (Model&& _Other) noexcept
{
	Meshes = std::move(_Other.Meshes);
}