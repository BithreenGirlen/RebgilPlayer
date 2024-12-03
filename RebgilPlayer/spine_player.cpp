
#include "spine_player.h"
#include "spine_loader.h"

CSpinePlayer::CSpinePlayer()
{

}

CSpinePlayer::~CSpinePlayer()
{

}

/*ファイル取り込み*/
bool CSpinePlayer::SetSpineFromFile(const std::vector<std::string>& atlasPaths, const std::vector<std::string>& skelPaths, bool bIsBinary)
{
	if (atlasPaths.size() != skelPaths.size())return false;
	ClearDrawables();

	for (size_t i = 0; i < atlasPaths.size(); ++i)
	{
		const std::string& strAtlasPath = atlasPaths.at(i);
		const std::string& strSkeletonPath = skelPaths.at(i);

		m_atlases.emplace_back(std::make_unique<spine::Atlas>(strAtlasPath.c_str(), &m_textureLoader));

		std::shared_ptr<spine::SkeletonData> skeletonData = bIsBinary ? spine_loader::readBinarySkeletonFromFile(strSkeletonPath.c_str(), m_atlases.back().get(), 1.f) : spine_loader::readTextSkeletonFromFile(strSkeletonPath.c_str(), m_atlases.back().get(), 1.f);
		if (skeletonData == nullptr)return false;

		m_skeletonData.emplace_back(skeletonData);
	}

	if (m_skeletonData.empty())return false;

	WorkOutDefualtSize();
	WorkOutDefaultScale();

	return SetupDrawer();
}
/*メモリ取り込み*/
bool CSpinePlayer::SetSpineFromMemory(const std::vector<std::string>& atlasData, const std::vector<std::string>& atlasPaths, const std::vector<std::string>& skelData, bool bIsBinary)
{
	if (atlasData.size() != skelData.size() || atlasData.size() != atlasPaths.size())return false;
	ClearDrawables();

	for (size_t i = 0; i < atlasData.size(); ++i)
	{
		const std::string& strAtlasDatum = atlasData.at(i);
		const std::string& strAtlasPath = atlasPaths.at(i);
		const std::string& strSkeletonData = skelData.at(i);

		m_atlases.emplace_back(std::make_unique<spine::Atlas>(strAtlasDatum.c_str(), static_cast<int>(strAtlasDatum.size()), strAtlasPath.c_str(), &m_textureLoader));

		std::shared_ptr<spine::SkeletonData> skeletonData = bIsBinary ? spine_loader::readBinarySkeletonFromMemory(strSkeletonData, m_atlases.back().get(), 1.f) : spine_loader::readTextSkeletonFromMemory(strSkeletonData, m_atlases.back().get(), 1.f);
		if (skeletonData == nullptr)return false;

		m_skeletonData.emplace_back(skeletonData);
	}

	if (m_skeletonData.empty())return false;

	WorkOutDefualtSize();
	WorkOutDefaultScale();

	return SetupDrawer();
}
/*拡縮変更*/
void CSpinePlayer::RescaleSkeleton(bool bUpscale, bool bWindowToBeResized)
{
	constexpr float kfMinScale = 0.15f;
	if (bUpscale)
	{
		m_fSkeletonScale += m_kfScalePortion;
	}
	else
	{
		m_fSkeletonScale -= m_kfScalePortion;
		if (m_fSkeletonScale < kfMinScale)m_fSkeletonScale = kfMinScale;
	}
	UpdateScaletonScale();
	if (bWindowToBeResized)
	{
		ResizeWindow();
		AdjustViewOffset();
		UpdatePosition();
	}
}
/*時間尺度変更*/
void CSpinePlayer::RescaleTime(bool bHasten)
{
	constexpr float kfTimeScalePortion = 0.05f;
	if (bHasten)
	{
		m_fTimeScale += kfTimeScalePortion;
	}
	else
	{
		m_fTimeScale -= kfTimeScalePortion;
	}
	if (m_fTimeScale < 0.f)m_fTimeScale = 0.f;

	UpdateTimeScale();
}
/*速度・尺度・視点初期化*/
void CSpinePlayer::ResetScale()
{
	m_fTimeScale = 1.0f;
	m_fSkeletonScale = m_fDefaultScale;
	m_fOffset = m_fDefaultOffset;
	m_fViewOffset = FPoint2{};

	UpdateScaletonScale();
	UpdateTimeScale();
	ResizeWindow();
	AdjustViewOffset();
	UpdatePosition();
}
/*視点移動*/
void CSpinePlayer::MoveViewPoint(int iX, int iY)
{
	m_fOffset.x += iX;
	m_fOffset.y += iY;
	UpdatePosition();
}
/*動作移行*/
void CSpinePlayer::ShiftAnimation()
{
	++m_nAnimationIndex;
	if (m_nAnimationIndex > m_animationNames.size() - 1)m_nAnimationIndex = 0;

	ClearAnimationTracks();

	for (size_t i = 0; i < m_drawables.size(); ++i)
	{
		spine::Animation* animation = m_skeletonData.at(i).get()->findAnimation(m_animationNames.at(m_nAnimationIndex).c_str());
		if (animation != nullptr)
		{
			m_drawables.at(i).get()->animationState->setAnimation(0, animation->getName(), true);
		}
	}
}
/*装い移行*/
void CSpinePlayer::ShiftSkin()
{
	++m_nSkinIndex;
	if (m_nSkinIndex > m_skinNames.size() - 1)m_nSkinIndex = 0;
	for (size_t i = 0; i < m_drawables.size(); ++i)
	{
		spine::Skin* skin = m_skeletonData.at(i).get()->findSkin(m_skinNames.at(m_nSkinIndex).c_str());
		if (skin != nullptr)
		{
			m_drawables.at(i).get()->skeleton->setSkin(skin);
		}
		m_drawables.at(i).get()->skeleton->setSlotsToSetupPose();
	}
}
/*乗算済み透過度有効・無効切り替え*/
void CSpinePlayer::SwitchPma()
{
	for (size_t i = 0; i < m_drawables.size(); ++i)
	{
		m_drawables.at(i).get()->SwitchPma();
	}
}
/*槽溝指定合成方法採択可否*/
void CSpinePlayer::SwitchBlendModeAdoption()
{
	for (size_t i = 0; i < m_drawables.size(); ++i)
	{
		m_drawables.at(i).get()->SwitchBlendModeAdoption();
	}
}
/*描画順切り替え*/
void CSpinePlayer::SwitchDrawOrder()
{
	m_bDrawOrderReversed ^= true;
}
/*現在の動作名と経過時間取得*/
std::string CSpinePlayer::GetCurrentAnimationNameWithTrackTime()
{
	for (const auto& pDrawable : m_drawables)
	{
		auto& tracks = pDrawable->animationState->getTracks();
		for (size_t i = 0; i < tracks.size(); ++i)
		{
			float fCurrentTime = tracks[i]->getTrackTime();
			spine::Animation* pAnimation = tracks[i]->getAnimation();
			if (pAnimation != nullptr)
			{
				std::string str = pAnimation->getName().buffer();
				str += "_" + std::to_string(fCurrentTime);
				return str;
			}
		}
	}

	return std::string();
}
/*槽溝名称引き渡し*/
std::vector<std::string> CSpinePlayer::GetSlotList()
{
	std::vector<std::string> slotNames;
	for (size_t i = 0; i < m_skeletonData.size(); ++i)
	{
		auto& slots = m_skeletonData.at(i).get()->getSlots();
		for (size_t ii = 0; ii < slots.size(); ++ii)
		{
			const std::string& strName = slots[ii]->getName().buffer();
			const auto iter = std::find(slotNames.begin(), slotNames.end(), strName);
			if (iter == slotNames.cend())slotNames.push_back(strName);
		}
	}
	return slotNames;
}
/*装い名称引き渡し*/
std::vector<std::string> CSpinePlayer::GetSkinList() const
{
	return m_skinNames;
}
/*動作名称引き渡し*/
std::vector<std::string> CSpinePlayer::GetAnimationList() const
{
	return m_animationNames;
}
/*描画除外リスト設定*/
void CSpinePlayer::SetSlotsToExclude(const std::vector<std::string>& slotNames)
{
	spine::Vector<spine::String> leaveOutList;
	for (const auto& slotName : slotNames)
	{
		leaveOutList.add(slotName.c_str());
	}

	for (size_t i = 0; i < m_drawables.size(); ++i)
	{
		m_drawables.at(i).get()->SetLeaveOutList(leaveOutList);
	}
}
/*装い合成*/
void CSpinePlayer::MixSkins(const std::vector<std::string>& skinNames)
{
	if (m_nSkinIndex >= m_skinNames.size())return;
	const auto& currentSkinName = m_skinNames.at(m_nSkinIndex);

	for (size_t i = 0; i < m_drawables.size(); ++i)
	{
		spine::Skin* skinToSet = m_skeletonData.at(i).get()->findSkin(currentSkinName.c_str());
		if (skinToSet != nullptr)
		{
			for (const auto& skinName : skinNames)
			{
				if (currentSkinName != skinName)
				{
					spine::Skin* skinToAdd = m_skeletonData.at(i).get()->findSkin(skinName.c_str());
					if (skinToAdd != nullptr)
					{
						skinToSet->addSkin(skinToAdd);
					}
				}
			}
			m_drawables.at(i).get()->skeleton->setSkin(skinToSet);
			m_drawables.at(i).get()->skeleton->setSlotsToSetupPose();
		}
	}
}
/*動作合成*/
void CSpinePlayer::MixAnimations(const std::vector<std::string>& animationNames)
{
	ClearAnimationTracks();

	if (m_nAnimationIndex >= m_animationNames.size())return;
	const auto& currentAnimationName = m_animationNames.at(m_nAnimationIndex);

	for (size_t i = 0; i < m_drawables.size(); ++i)
	{
		if (m_skeletonData.at(i).get()->findAnimation(currentAnimationName.c_str()) == nullptr)continue;

		int iTrack = 1;
		for (const auto& animationName : animationNames)
		{
			if (animationName != currentAnimationName)
			{
				spine::Animation* animation = m_skeletonData.at(i).get()->findAnimation(animationName.c_str());
				if (animation != nullptr)
				{
					m_drawables.at(i).get()->animationState->addAnimation(iTrack, animation, false, 0.f);
					++iTrack;
				}
			}
		}
	}
}
/*描画除外是否関数登録*/
void CSpinePlayer::SetSlotExclusionCallback(bool(*pFunc)(const char*, size_t))
{
	for (const auto& pDrawable : m_drawables)
	{
		pDrawable->SetLeaveOutCallback(pFunc);
	}
}
/*消去*/
void CSpinePlayer::ClearDrawables()
{
	m_drawables.clear();
	m_atlases.clear();
	m_skeletonData.clear();

	m_animationNames.clear();
	m_nAnimationIndex = 0;

	m_skinNames.clear();
	m_nSkinIndex = 0;
}
/*描画器設定*/
bool CSpinePlayer::SetupDrawer()
{
	for (size_t i = 0; i < m_skeletonData.size(); ++i)
	{
		m_drawables.emplace_back(std::make_shared<CSpineDrawable>(m_skeletonData.at(i).get()));

		CSpineDrawable* drawable = m_drawables.at(i).get();
		drawable->timeScale = 1.0f;
		drawable->skeleton->setPosition(m_fBaseSize.x / 2, m_fBaseSize.y / 2);
		drawable->skeleton->updateWorldTransform();

		auto& animations = m_skeletonData.at(i).get()->getAnimations();
		for (size_t ii = 0; ii < animations.size(); ++ii)
		{
			const std::string& strAnimationName = animations[ii]->getName().buffer();
			const auto iter = std::find(m_animationNames.begin(), m_animationNames.end(), strAnimationName);
			if (iter == m_animationNames.cend())m_animationNames.push_back(strAnimationName);
		}

		auto& skins = m_skeletonData.at(i).get()->getSkins();
		for (size_t ii = 0; ii < skins.size(); ++ii)
		{
			const std::string& strName = skins[ii]->getName().buffer();
			const auto iter = std::find(m_skinNames.begin(), m_skinNames.end(), strName);
			if (iter == m_skinNames.cend())m_skinNames.push_back(strName);
		}
	}

	if (!m_animationNames.empty())
	{
		for (size_t i = 0; i < m_skeletonData.size(); ++i)
		{
			spine::Animation* animation = m_skeletonData.at(i).get()->findAnimation(m_animationNames.at(0).c_str());
			if (animation != nullptr)
			{
				m_drawables.at(i).get()->animationState->setAnimation(0, animation->getName(), true);
			}
		}
	}

	ResetScale();

	return m_animationNames.size() > 0;
}
/*基準寸法・位置算出*/
void CSpinePlayer::WorkOutDefualtSize()
{
	if (m_skeletonData.empty())return;

	float fMaxSize = 0.f;
	const auto CompareDimention = [this, &fMaxSize](float fWidth, float fHeight)
		-> bool
		{
			if (fWidth > 0.f && fHeight > 0.f && fWidth * fHeight > fMaxSize)
			{
				m_fBaseSize.x = fWidth;
				m_fBaseSize.y = fHeight;
				fMaxSize = fWidth * fHeight;
				return true;
			}

			return false;
		};

	for (const auto& pSkeletonData : m_skeletonData)
	{
		if (pSkeletonData.get()->getWidth() > 0 && pSkeletonData.get()->getHeight())
		{
			CompareDimention(pSkeletonData.get()->getWidth(), pSkeletonData.get()->getHeight());
		}
		else
		{
			/*Why spine::Skin lacks searching methods based on its own spine::Vector<spine::Attachment*>?*/
			const auto FindDefaultSkinAttachment = [&pSkeletonData]()
				-> spine::Attachment*
				{
					spine::Skin::AttachmentMap::Entries attachmentMapEntries = pSkeletonData.get()->getDefaultSkin()->getAttachments();
					for (; attachmentMapEntries.hasNext();)
					{
						spine::Skin::AttachmentMap::Entry attachmentMapEntry = attachmentMapEntries.next();
						if (attachmentMapEntry._slotIndex == 0)
						{
							return attachmentMapEntry._attachment;
						}
					}
					return nullptr;
				};

			spine::Attachment* pAttachment = FindDefaultSkinAttachment();
			if (pAttachment == nullptr)continue;

			if (pAttachment->getRTTI().isExactly(spine::RegionAttachment::rtti))
			{
				spine::RegionAttachment* pRegionAttachment = (spine::RegionAttachment*)pAttachment;

				CompareDimention(pRegionAttachment->getWidth() * pRegionAttachment->getScaleX(), pRegionAttachment->getHeight() * pRegionAttachment->getScaleY());
			}
			else if (pAttachment->getRTTI().isExactly(spine::MeshAttachment::rtti))
			{
				spine::MeshAttachment* pMeshAttachment = (spine::MeshAttachment*)pAttachment;

				spine::SlotData* pSlotData = pSkeletonData.get()->findSlot(pAttachment->getName());

				float fScaleX = pSlotData != nullptr ? pSlotData->getBoneData().getScaleX() : 1.f;
				float fScaleY = pSlotData != nullptr ? pSlotData->getBoneData().getScaleY() : 1.f;

				CompareDimention(pMeshAttachment->getWidth() * fScaleX, pMeshAttachment->getHeight() * fScaleY);
			}
		}
	}
}
/*位置適用*/
void CSpinePlayer::UpdatePosition()
{
	for (size_t i = 0; i < m_drawables.size(); ++i)
	{
		m_drawables.at(i).get()->skeleton->setPosition(m_fBaseSize.x / 2 - m_fOffset.x - m_fViewOffset.x, m_fBaseSize.y / 2 - m_fOffset.y - m_fViewOffset.y);
	}
}
/*尺度適用*/
void CSpinePlayer::UpdateScaletonScale()
{
	for (size_t i = 0; i < m_drawables.size(); ++i)
	{
		m_drawables.at(i).get()->skeleton->setScaleX(m_fSkeletonScale);
		m_drawables.at(i).get()->skeleton->setScaleY(m_fSkeletonScale);
	}
}
/*速度適用*/
void CSpinePlayer::UpdateTimeScale()
{
	for (size_t i = 0; i < m_drawables.size(); ++i)
	{
		m_drawables.at(i).get()->timeScale = m_fTimeScale;
	}
}
/*合成動作消去*/
void CSpinePlayer::ClearAnimationTracks()
{
	for (size_t i = 0; i < m_drawables.size(); ++i)
	{
		const auto& trackEntry = m_drawables.at(i).get()->animationState->getTracks();
		for (size_t iTrack = 1; iTrack < trackEntry.size(); ++iTrack)
		{
			m_drawables.at(i).get()->animationState->setEmptyAnimation(iTrack, 0.f);
		}
	}
}