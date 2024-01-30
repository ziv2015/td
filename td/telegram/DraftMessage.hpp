//
// Copyright Aliaksei Levin (levlam@telegram.org), Arseny Smirnov (arseny30@gmail.com) 2014-2024
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include "td/telegram/DraftMessage.h"

#include "td/telegram/InputMessageText.hpp"
#include "td/telegram/MessageInputReplyTo.hpp"
#include "td/telegram/Version.h"

#include "td/utils/tl_helpers.h"

namespace td {

template <class StorerT>
void DraftMessage::store(StorerT &storer) const {
  bool has_input_message_text = !input_message_text_.is_empty();
  bool has_message_input_reply_to = !message_input_reply_to_.is_empty();
  bool has_local_content = local_content_ != nullptr;
  BEGIN_STORE_FLAGS();
  STORE_FLAG(has_input_message_text);
  STORE_FLAG(has_message_input_reply_to);
  STORE_FLAG(has_local_content);
  END_STORE_FLAGS();
  td::store(date_, storer);
  if (has_input_message_text) {
    td::store(input_message_text_, storer);
  }
  if (has_message_input_reply_to) {
    td::store(message_input_reply_to_, storer);
  }
  if (has_local_content) {
    store_draft_message_content(local_content_.get(), storer);
  }
}

template <class ParserT>
void DraftMessage::parse(ParserT &parser) {
  bool has_legacy_reply_to_message_id;
  bool has_input_message_text = true;
  bool has_message_input_reply_to = false;
  bool has_local_content = false;
  if (parser.version() >= static_cast<int32>(Version::SupportRepliesInOtherChats)) {
    has_legacy_reply_to_message_id = false;
    BEGIN_PARSE_FLAGS();
    PARSE_FLAG(has_input_message_text);
    PARSE_FLAG(has_message_input_reply_to);
    PARSE_FLAG(has_local_content);
    END_PARSE_FLAGS();
  } else {
    has_legacy_reply_to_message_id = true;
  }
  td::parse(date_, parser);
  if (has_legacy_reply_to_message_id) {
    MessageId legacy_reply_to_message_id;
    td::parse(legacy_reply_to_message_id, parser);
    message_input_reply_to_ = MessageInputReplyTo{legacy_reply_to_message_id, DialogId(), FormattedText(), 0};
  }
  if (has_input_message_text) {
    td::parse(input_message_text_, parser);
  }
  if (has_message_input_reply_to) {
    td::parse(message_input_reply_to_, parser);
  }
  if (has_local_content) {
    parse_draft_message_content(local_content_, parser);
  }
}

}  // namespace td
