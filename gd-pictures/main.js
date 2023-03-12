const Promise = require('bluebird');
const fs = require('fs');
const {argv} = require('yargs');
const rp = require('request-promise');
const path = require('path');
const cheerio = require('cheerio');
const iconv = require('iconv-lite');

const TIMEOUT = argv.timeout || 2000;
const MAX_IMAGE_COUNT = Number(argv.images) || 20;
const SUGGESTED_CLASS = argv.suggestedClass || 'a.dloBPe.M3vVJe';
const ALTERNATIVE_CLASS = argv.alternativeClass || '.X6ZCif';
const URI = 'https://www.google.co.jp/search?tbm=isch';
const GD_URI = 'gdlookup://localhost/';

const ORIGINAL_HREF = 'https://www.google.co.jp';
const FULL_URI = `https://www.google.com/search?tbm=isch&q=${argv.input}&hl=${argv.lang}`;
const QUERY_URI = `https://www.google.com/search?tbm=isch&hl=${argv.lang}&q=`;
const HEADER_URI = `${URI}&q=${argv.input}&hl=${argv.lang}`;

const primarySelector = 'td';

function renderMarkup(input) {
  let base = ``;
  let markup = [];
  if (input.imageGrid) {
    markup.push(`
      <section id="image-grid" class="japanese_gothic focus" lang="ja">${input.imageGrid}</section>
    `);
  }

  console.log(`<!--<div id="golden-images">${base + markup.join('')}</div>-->`)
  return `<div id="golden-images">${base + markup.join('')}</div>`;
}

function round(input) {
  return parseFloat(Math.round(input * 1000) / 1000).toFixed(3);
}


function sanitize(input) {
  // Need to figure out what Jisho.org uri breaks on
  return input;
}

function getMainComponent($) {
  const content = $(primarySelector);
  if (!content.length) {
    return {};
  }
  let newContent = '';
  let misses = 0; //bad selector, so skip first 3 non results
  // eslint-disable-next-line func-names
  let imageCount = 0;
  content.find('a img').slice(misses).each(function () {
    if(imageCount >= MAX_IMAGE_COUNT){

      return;
    }
    const $link = $(`<a></a>`)
    const $img = $(this);
    const oldSrc = $img.closest('a').attr('href');
    const newSrc = ORIGINAL_HREF + oldSrc;

    $link.attr('href', newSrc);
    $link.addClass('image-container');
    $newImg = $('<img />');
    $newImg.attr('src', $img.attr('src'));
    $newImg.attr('title', $img.attr('alt'));
    const c = $link.append($newImg);
    newContent += `${$.html(c)}`;
    imageCount++;
  });
  return {imageGrid: `<div class="image-list">${newContent}</div>`, hasImageGrid:!!newContent};
}

function getExactBlock($) {
  const content = $(primarySelector);
  if (!content.length) {
    return {};
  }
  let newContent;
  return {exactBlock: `<div>${newContent}</div>`};
}

function getSuggested ($) {
  const oldSrc = $(SUGGESTED_CLASS);
  const newSrc = GD_URI + oldSrc.text();
  let newContent = oldSrc.attr('href', newSrc);

  return {
    suggested: `<div><h3>${newContent}</h3></div>`,
    hasSuggested: !!oldSrc[0]
  }
}

function getAlternatives ($) {
  const src = $(ALTERNATIVE_CLASS);
  let hasAlternative = false;
  src.find('a').each(function() {
    hasAlternative = true;
    const alternative = $(this).text();
    const newHref = GD_URI + encodeURIComponent(`${argv.input} ${alternative}`);
    const $this = $(this);
    $this.attr('href', newHref);
  })
  return {
    alternative: `<div><h3>${src}</h3></div>`,
    hasAlternative
  }
}

function sanitizeRespose($) {
  return Object.assign({}, getExactBlock($), getMainComponent($), getSuggested($), getAlternatives($));
}

function output(input) {
  if (!Object.keys(input).length) {
    return null;
  }
  const content = renderMarkup(input);

  console.log(content);
}

function getContent(args) {
  const options = {
    uri: encodeURI(FULL_URI),
    encoding: null,

    transform(body) {
      body = iconv.decode(new Buffer(body), 'Shift_JIS')
      return cheerio.load(body);
    },
  };
  return rp(options)
    .then($ => sanitizeRespose($))
    .catch(err => err);
}

getContent(sanitize(argv)).then(output);
