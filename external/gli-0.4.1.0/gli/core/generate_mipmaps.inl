///////////////////////////////////////////////////////////////////////////////////
/// OpenGL Image (gli.g-truc.net)
///
/// Copyright (c) 2008 - 2013 G-Truc Creation (www.g-truc.net)
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
/// 
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
/// 
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
///
/// @ref core
/// @file gli/core/generate_mipmaps.inl
/// @date 2010-09-27 / 2013-01-12
/// @author Christophe Riccio
///////////////////////////////////////////////////////////////////////////////////

namespace gli
{
	template <>
	inline texture2D generateMipmaps
	(
		texture2D const & Texture, 
		texture2D::size_type const & BaseLevel
	)
	{
		assert(BaseLevel < Texture.levels());
		texture2D::format_type const Format = Texture.format();
		texture2D::size_type const ValueSize = gli::block_size(Format);
		texture2D::size_type const Components = gli::component_count(Format);

		assert(Format == R8U || Format == RG8U || Format == RGB8U || Format == RGBA8U);
		texture2D::size_type Levels = std::size_t(glm::log2(float(glm::compMax(Texture[0].dimensions())))) + 1;

		texture2D Result(Levels, Format, Texture.dimensions());

		for(texture2D::size_type Level = BaseLevel; Level < Levels - 1; ++Level)
		{
			std::size_t BaseWidth = Result[Level + 0].dimensions().x;
			glm::byte * DataSrc = reinterpret_cast<glm::byte *>(Result[Level + 0].data());

			texture2D::dimensions_type LevelDimensions = texture2D::dimensions_type(Result[Level + 0].dimensions()) >> texture2D::dimensions_type(1);
			LevelDimensions = glm::max(LevelDimensions, texture2D::dimensions_type(1));

			std::vector<detail::storage::data_type> DataDst(detail::storage::size_type(glm::compMul(LevelDimensions)) * Components);

			for(std::size_t j = 0; j < LevelDimensions.y; ++j)
			for(std::size_t i = 0; i < LevelDimensions.x;  ++i)
			for(std::size_t c = 0; c < Components; ++c)
			{
				std::size_t x = (i << 1);
				std::size_t y = (j << 1);

				std::size_t Index00 = ((x + 0) + (y + 0) * BaseWidth) * Components + c;
				std::size_t Index01 = ((x + 0) + (y + 1) * BaseWidth) * Components + c;
				std::size_t Index11 = ((x + 1) + (y + 1) * BaseWidth) * Components + c;
				std::size_t Index10 = ((x + 1) + (y + 0) * BaseWidth) * Components + c;

				glm::u32 Data00 = reinterpret_cast<glm::byte *>(DataSrc)[Index00];
				glm::u32 Data01 = reinterpret_cast<glm::byte *>(DataSrc)[Index01];
				glm::u32 Data11 = reinterpret_cast<glm::byte *>(DataSrc)[Index11];
				glm::u32 Data10 = reinterpret_cast<glm::byte *>(DataSrc)[Index10];

				glm::byte Result = (Data00 + Data01 + Data11 + Data10) >> 2;
				glm::byte * Data = &DataDst[0];

				*(Data + ((i + j * LevelDimensions.x) * Components + c)) = Result;
			}

			memcpy(Result[Level + 1].data(), &DataDst[0], DataDst.size());
		}

		return Result;
	}

}//namespace gli
