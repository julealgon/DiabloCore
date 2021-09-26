#include "dvlnet/packet.h"

namespace devilution {
namespace net {

const char *packet_type_to_string(uint8_t packetType)
{
	switch (packetType) {
	case PT_MESSAGE:
		return "PT_MESSAGE";
	case PT_TURN:
		return "PT_TURN";
	case PT_JOIN_REQUEST:
		return "PT_JOIN_REQUEST";
	case PT_JOIN_ACCEPT:
		return "PT_JOIN_ACCEPT";
	case PT_CONNECT:
		return "PT_CONNECT";
	case PT_DISCONNECT:
		return "PT_DISCONNECT";
	case PT_INFO_REQUEST:
		return "PT_INFO_REQUEST";
	case PT_INFO_REPLY:
		return "PT_INFO_REPLY";
	default:
		return nullptr;
	}
}

wrong_packet_type_exception::wrong_packet_type_exception(std::initializer_list<packet_type> expectedTypes, std::uint8_t actual)
{
	message_ = "Expected packet of type ";
	const auto appendPacketType = [this](std::uint8_t t) {
		const char *typeStr = packet_type_to_string(t);
		if (typeStr != nullptr)
			message_.append(typeStr);
		else
			message_.append(std::to_string(t));
	};

	constexpr char KJoinTypes[] = " or ";
	for (const packet_type t : expectedTypes) {
		appendPacketType(t);
		message_.append(KJoinTypes);
	}
	message_.resize(message_.size() - (sizeof(KJoinTypes) - 1));
	message_.append(", got");
	appendPacketType(actual);
}

namespace {

void CheckPacketTypeOneOf(std::initializer_list<packet_type> expectedTypes, std::uint8_t actualType)
{
	for (std::uint8_t packetType : expectedTypes)
		if (actualType == packetType)
			return;
	throw wrong_packet_type_exception(expectedTypes, actualType);
}

} // namespace

const buffer_t &packet::Data()
{
	if (!have_decrypted || !have_encrypted)
		ABORT();
	return encrypted_buffer;
}

packet_type packet::Type()
{
	if (!have_decrypted)
		ABORT();
	return m_type;
}

plr_t packet::Source() const
{
	if (!have_decrypted)
		ABORT();
	return m_src;
}

plr_t packet::Destination() const
{
	if (!have_decrypted)
		ABORT();
	return m_dest;
}

const buffer_t &packet::Message()
{
	if (!have_decrypted)
		ABORT();
	CheckPacketTypeOneOf({ PT_MESSAGE }, m_type);
	return m_message;
}

turn_t packet::Turn()
{
	if (!have_decrypted)
		ABORT();
	CheckPacketTypeOneOf({ PT_TURN }, m_type);
	return m_turn;
}

cookie_t packet::Cookie()
{
	if (!have_decrypted)
		ABORT();
	CheckPacketTypeOneOf({ PT_JOIN_REQUEST, PT_JOIN_ACCEPT }, m_type);
	return m_cookie;
}

plr_t packet::NewPlayer()
{
	if (!have_decrypted)
		ABORT();
	CheckPacketTypeOneOf({ PT_JOIN_ACCEPT, PT_CONNECT, PT_DISCONNECT }, m_type);
	return m_newplr;
}

const buffer_t &packet::Info()
{
	if (!have_decrypted)
		ABORT();
	CheckPacketTypeOneOf({ PT_JOIN_REQUEST, PT_JOIN_ACCEPT, PT_CONNECT, PT_INFO_REPLY }, m_type);
	return m_info;
}

leaveinfo_t packet::LeaveInfo()
{
	if (!have_decrypted)
		ABORT();
	CheckPacketTypeOneOf({ PT_DISCONNECT }, m_type);
	return m_leaveinfo;
}

void packet_in::Create(buffer_t buf)
{
	if (have_encrypted || have_decrypted)
		ABORT();
	encrypted_buffer = std::move(buf);
	have_encrypted = true;
}

void packet_in::Decrypt()
{
	if (!have_encrypted)
		ABORT();
	if (have_decrypted)
		return;
	
	if (encrypted_buffer.size() < sizeof(packet_type) + 2 * sizeof(plr_t))
		throw packet_exception();
	decrypted_buffer = encrypted_buffer;

	process_data();

	have_decrypted = true;
}

void packet_out::Encrypt()
{
	if (!have_decrypted)
		ABORT();
	if (have_encrypted)
		return;

	process_data();

	have_encrypted = true;
}

packet_factory::packet_factory(std::string pw)
{

}

} // namespace net
} // namespace devilution
